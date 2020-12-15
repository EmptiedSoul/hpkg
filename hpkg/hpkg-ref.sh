#!/bin/bash

##    This file is part of hpkg.

##    hpkg is free software: you can redistribute it and/or modify
##    it under the terms of the GNU General Public License as published by
##    the Free Software Foundation, either version 3 of the License, or
##    (at your option) any later version.

##    hpkg is distributed in the hope that it will be useful,
##    but WITHOUT ANY WARRANTY; without even the implied warranty of
##    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##    GNU General Public License for more details.

##    You should have received a copy of the GNU General Public License
##    along with hpkg.  If not, see <https://www.gnu.org/licenses/>.

# ERROR CODES:
##	1 - archive not found
##	2 - unable to unpack
##	4 - archive unpacked but corrupted
##	8 - (installer) cannot resolve dependencies
##	16 - (installer) conflicting packages

# Macro for /dev/null
exec 3>errors+supressed.log


### Here starts declaration of output functions
__log(){
	case $3 in
		REQUEST)
			local COLOR="\e[36m"
			;;
		ERROR)
			local COLOR="\e[31m"
			;;
		SUCCESS)
			local COLOR="\e[32m"
			;;
		CALL)
			local COLOR="\e[35m"
	esac
	echo -e "[ $(date +%F%t%H:%M:%S) ] [ $4 ] [ $COLOR$3\e[0m ] \e[36m$1\e[0m \e[31m$2\e[0m\t: $5" >> /var/log/hpkg/pkg.log
}
__error(){

	echo -e "\a\033[41;1mError:\033[0m \033[32;1m$1:\033[0m\033[34;1m $2 \033[0m" 1>&2
	rm -rf HPKG_PAYLOAD 2>&3
	if [ "$TRACE" == "YES" ]; then
	if ! [ -z "$(cat errors+supressed.log)" ]; then
		echo "Trace:" 1>&2
		cat errors+supressed.log | tail 1>&2
		rm -f errors+supressed.log
	fi
	fi
	__log "(hpkg) $1" "" "ERROR" "$3" "$2"
	exit $3

}

__bigPointerWithTime(){

	echo -e "\033[34;1m==>\033[0m $1 - \033[32;1mDONE in $SECONDS seconds\033[0m"

}

__bigPointer(){

	echo -e "\033[34;1m==>\033[0m $1"

}

__middlePointer(){

	echo -e "\033[33;1m  ->\033[0m $1"

}

__littlePointer(){
	if [ "$2" == "bad" ] ; then
		echo -e "\033[31;1m     >\033[0m $1"
	else
		echo -e "\033[32;1m     >\033[0m $1"
	fi

}

__warn(){

	echo -e "\033[34;1mhpkg:\033[0m $1" 1>&2

}

### Output functions declaration ends

### Core functions declaration starts

__initialize(){
# Setting Defaults
KEEP_CACHE="YES"
SLACK="NO"
QUIET="NO"
ROOT="/"
PIPE="YES"
COMPRESS="xz"
ENCRYPT="password"
MIRRORLIST="/etc/hpkg/mirror.list"
CONFIG="/etc/hpkg/hpkg.conf"
# Overruling defaults by config
	if [ -e "$CONFIG" ] ; then
		source "$CONFIG"
	fi
# Supressing output
	if [ "$QUIET" == "YES" ] ; then
		exec 1>/dev/null
	fi
}

__unpack(){
# Setting working variables
	stat="$1"
	NAME=$(echo $stat | sed 's/.hard//')
	WORKNAME="HPKG_PAYLOAD"
# Primary validation 
	__bigPointer "Preparing to unpack $stat..."
	
	if ! [ -e "$stat" ] ; then	
		__littlePointer "$stat Not Found!" "bad"
		__error "$stat" "No such file" "1"
	fi
# If valid	
# Setting PASS
	if [ "$ENCRYPT" == "password" ] ; then
		if [ "$2" == "withPass" ] ; then
			PASSWORD="$3"
		else
			read -s -p "Password: " PASSWORD
			echo -e "\033[32;1m Accepted \033[0m"
		fi
	fi
# Unpacking 
	__middlePointer "Decompressing $stat..."

	gpg --verify $1 1>&3 2>&3 || __error "Decryption" "unable to verify package" "2"

	if [ "$COMPRESS" == "xz" ] ; then
		gpg --batch --yes --passphrase $MODPASS -d $1 2>&3 | unxz 2>&3 | tar xv 2>ers.list | awk '{print "\033[32;1m    > \033[0mDecompressed \033[34;1m "$0" \033[0m"}'  
	elif [ "$COMPRESS" == "bz2" ]; then
		gpg --batch --yes --passphrase $MODPASS -d $1 2>&3 | bunzip 2>&3 | tar xv 2>ers.list | awk '{print "\033[32;1m    > \033[0mDecompressed \033[34;1m "$0" \033[0m"}' 
	fi

	# Ensuring that package extracted properly (Dumbest way, replace it)

    if [ -e "ers.list" ]; then
    	if grep -q "tar:" ers.list ; then
    		RES=1
    	else
    		RES=0
    	fi
    	rm -rf ers.list 2>&3
    else
    	RES=0
    	rm -rf ers.list 2>&3
    fi
    if [ $RES -eq 0 ]; then 
    	true
    else
    	__error "Decryption" "Wrong password (missing keys)" "2"
    fi

    # Starting working with extracted package itself
  
    if ! [ -e "$WORKNAME"/.ARCH_TYPE ] ; then
    	rm -rf "$WORKNAME"
    	__error "Archive" "Corrupted data" "4"
    else
    	__bigPointer "Initializating $stat..."
    	__middlePointer "Reading $stat metadata..."
    	ARCH_TYPE=$(cat "$WORKNAME"/.ARCH_TYPE)
    	ACCESS=$(cat "$WORKNAME"/.ACCESS)
		echo -e "\033[32;1m\tArchive Name:\033[0m $stat"
		echo -e "\033[32;1m\tArchive Type:\033[0m $ARCH_TYPE"
		echo -e "\033[32;1m\tArchive Access Type:\033[0m $ACCESS"

		case $ARCH_TYPE in
			executable)
				__warn "This archive marked as 'executable', executing..."
    			__bigPointer "Executing $stat"
    			if [ -e "$WORKNAME"/MAIN ] ; then
    				__littlePointer "Found !"
    				./MAIN        
				else
    				__error "$stat" "No main script" "4"
    			fi
    			if [ $ACCESS == "public" ] ; then
    				mv -f $WORKNAME $NAME
            	else 
            	    rm -rf $WORKNAME
            	fi
            	__bigPointerWithTime "Unpacking & Executing"
            ;;
            installer)
				__log "(hpkg) $0" "$1" "REQUEST" "-" "installing $1"
				__warn "This archive marked as 'installer', installing..."
				__bigPointer "Installing $stat"
    			VER=$(cat "$WORKNAME"/.VERSION)
    			PKG=$(cat "$WORKNAME"/.PKGNAME)
    			CONFL=$(cat "$WORKNAME/.CONFLICTS")
    			DEPS=$(cat "$WORKNAME/.DEPENDS")
    			MAINTAINER=$(cat "$WORKNAME/.MAINTAINER")
    			FLST="/var/hpkg/installed-files/$PKG-$VER.list"
    			DEPSHERE=1
    			CONFLICTS=0
    			#ROOT="/"
	
    			if [ "$SLACK" == "NO" ] ; then

    			__bigPointer "Resolving dependencies for $stat..." 1>&2
    			for depsfind in $DEPS
				do
					if ! grep "^$depsfind" -qr /var/hpkg/packages-provides/ ; then
						if ! find /var/hpkg/packages/ -name "$depsfind.info" | grep "."
  						then
  							__log "(hpkg) $0" "$1" "CALL" "-" "calling hardman to resolve dependences"
							if [ "$USETEMPDIR" == "NO" ]; then
								tempdir="/var/cache/hpkg/payload.$RANDOM"
								mkdir -p $tempdir
							else
								tempdir=$(mktemp -d)
							fi 
  							mv HPKG_PAYLOAD $tempdir
							if ! echo | hardman install "$depsfind_" ; then
								__error "$stat" "$PKG depends on: $depsfind. However it not installed. Stop" "8"
							fi
  							mv $tempdir/HPKG_PAYLOAD HPKG_PAYLOAD
							rm -rf $tempdir
						fi
					fi
				done	 	
				else
					__warn "Slackware mode enabled: dependecies tracking disabled"
				fi

				__bigPointer "Checking conflicts for $stat..." 1>&2
    			for conflictfind in $CONFL
				do
					if find /var/hpkg/packages/ -name "$conflictfind.info" | grep -q "."
  				    then
  						__error "$stat" "$PKG conflicts with $conflictfind. Stop" "16"
  					fi
				done 
				if grep -qr "^$PKG" /var/hpkg/packages-conflicts/ ; then
					__error "$stat" "$(grep -r "^$PKG" /var/hpkg/packages-conflicts/ | sed -e 's/:.*//' -e 's/.CONFLICTS//' -e 's|.*/||') conflicts with $PKG. Stop" "16"
				fi

 	           	if [ -e "$WORKNAME"/preinstall ] ; then
	           	 	cd "$WORKNAME"
	           	 	__log "(hpkg) $0" "$1" "CALL" "-" "running preinstall script"
	           	    ./preinstall
	           	    cd ..
 	           	fi
 	           		cd "$WORKNAME"
            	
	           	     if tar xpf PAYLOAD.tar -C "$ROOT" --owner=0 --group=0 ; then
	           	     	tar --list -f PAYLOAD.tar > "$FLST"
	           	     	echo "Package: $PKG" > /var/hpkg/packages/"$PKG.info"
	           	     	echo "Version: $VER" >> /var/hpkg/packages/"$PKG.info"
	           	     	echo "Files: $FLST" >> /var/hpkg/packages/"$PKG.info"
	           	     	echo "Conflicts: $CONFL" >> /var/hpkg/packages/"$PKG.info"
	           	     	echo "Depends: $DEPS" >> /var/hpkg/packages/"$PKG.info"
	           	     	echo "Provides: $PROV" >> /var/hpkg/packages/"$PKG.info"
	           	     	echo "Maintainer: $MAINTAINER" >> /var/hpkg/packages/"$PKG.info"
	           	     	cat .DEPENDS > /var/hpkg/packages-dependencies/"$PKG.DEPENDS"
	           	     	cat .PROVIDES > /var/hpkg/packages-provides/"$PKG.PROVIDES" 2>&3
	           	     	cat .CONFLICTS > /var/hpkg/packages-conflicts/"$PKG.CONFLICTS"
	           	     	mkdir -p /var/hpkg/builds/"$PKG"
	           	     	mv -f Buildfile /var/hpkg/builds/"$PKG"/ 2>&3
	           	     	mv -f preremove /var/hpkg/scripts/"$PKG".preremove 2>&3
	           	     	mv -f postremove /var/hpkg/scripts/"$PKG".postremove 2>&3
	           	     	mv -f preinstall /var/hpkg/scripts/"$PKG".preinstall 2>&3
	           	     	mv -f postinstall /var/hpkg/scripts/"$PKG".postinstall 2>&3
	           	     	mv -f .DESCRIPTION /var/hpkg/desc/"$PKG".desc 2>&3
	           	     fi
	           	     cd ..
 	           	if [ -e "$WORKNAME"/postinstall ] ; then
 	           	     cd "$WORKNAME"
 	           	     __log "(hpkg) $0" "$1" "CALL" "-" "running postinstall script"
	           	     ./postinstall
	           	     cd ..
	           	fi
	           	if [ $ACCESS == "public" ] ; then
	           	     mv -f $WORKNAME $NAME
 	           	else 
 	           	    	#echo "No access type specified: set as default"
 	           	    rm -rf $WORKNAME
 	          	 	fi
 	          	 	__log "(hpkg) $0" "$1" "SUCCESS" "0" "$1 installed"
 	          	 	__bigPointerWithTime "Installing $stat"
			;;
			container)
				__warn "This archive marked as 'container', all done!"
    			mv -f $WORKNAME $NAME
    			__bigPointerWithTime "Unpacking"
			;;
			port)
				# Temporary removed
				true
			;;
		esac
		
	
	fi

}

### Core functions declaration ends

### Entry point

__initialize


__unpack $1
