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

##    CONTRIBUTING RULES
##    1. Check is your code valid
##    2. Check your code with ShellCheck
##    3. Beautify your code
##    4. Generate diff
##    5. Mail your patch to maintainer

# Macro for /dev/null
exec 3>errors+supressed.log


### Here starts declaration of output functions

__error(){

	echo -e "\a\033[41;1mError:\033[0m \033[32;1m$1:\033[0m\033[34;1m $2 \033[0m" 1>&2
	rm -rf HPKG_PAYLOAD 2>&3
	if [ "$TRACE" == "YES" ]; then
	if ! [ -z "$(cat errors+supressed.log)" ]; then
		echo "Trace:" 1>&2
		cat errors+supressed.log | tail 1>&2
	fi
	fi
	exit 1

}

__bigPointerWithTime(){

	echo -e "\033[34;1m==>\033[0m $1 - \033[32;1mTARGET REACHED in $SECONDS seconds\033[0m"

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

__credits(){

	echo "hpkg - .hard packager - v. 0.9.0"
		echo
		echo ".hard - hard archives replacing debs"
		echo
		echo "Written by Emptied Soul and contributors"

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
	__middlePointer "Searching $stat..."
	
	if [ -e "$stat" ] ; then	
		__littlePointer "Found $stat !"
	else
		__littlePointer "$stat Not Found!" "bad"
		__error "$stat" "No such file"
	fi

	__middlePointer "Checking $stat extension..."
	
	if grep -q ".hard" <<<$stat; then
		__littlePointer ".hard !"
	else
		__littlePointer "Not .hard !" "bad"
		__error "$stat" "It is not .hard archive"
	fi
# If valid	
	__bigPointerWithTime "Preparing to unpack $stat"
# Setting PASS
	if [ "$ENCRYPT" == "password" ] ; then
		if [ "$2" == "withPass" ] ; then
			PASSWORD="$3"
		else
			read -s -p "Password: " PASSWORD
			echo -e "\033[32;1m Accepted \033[0m"
		fi
	fi
# Dumb hardening (Package cant be extracted manually)
	let "MODPASS = PASSWORD + 11111111"
# Unpacking 
	__middlePointer "Decompressing $stat..."
	if [ "$COMPRESS" == "xz" ] ; then
		gpg --batch --yes --passphrase $MODPASS -d $1 2>&3 | unxz 2>&3 | tar xv 2>ers.list | awk '{print "\033[32;1m    > \033[0mDecompressed \033[34;1m "$0" \033[0m"}'  #echo "Error"; exit 1;
	elif [ "$COMPRESS" == "bz2" ]; then
		gpg --batch --yes --passphrase $MODPASS -d $1 2>&3 | bunzip 2>&3 | tar xv 2>ers.list | awk '{print "\033[32;1m    > \033[0mDecompressed \033[34;1m "$0" \033[0m"}'  #echo "Error"; exit 1;
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
    	__error "Decryption" "Wrong password (missing keys)"
    fi

    # Starting working with extracted package itself

    __middlePointer "Searching .ARCH_TYPE in $1..."
  
    if ! [ -e "$WORKNAME"/.ARCH_TYPE ] ; then
    	rm -rf "$WORKNAME"
    	__error "Archive" "Corrupted data"
    else
    	__littlePointer "Found .ARCH_TYPE !"
    	__bigPointerWithTime "Unpacking $stat"
    	__bigPointer "Initializating $stat..."
    	__middlePointer "Reading $stat metadata..."
    	ARCH_TYPE=$(cat "$WORKNAME"/.ARCH_TYPE)
    	ACCESS=$(cat "$WORKNAME"/.ACCESS)
		echo -e "\033[32;1m\tArchive Name:\033[0m $stat"
		echo -e "\033[32;1m\tArchive Type:\033[0m $ARCH_TYPE"
		echo -e "\033[32;1m\tArchive Access Type:\033[0m $ACCESS"
		__bigPointerWithTime "Initializating $stat"

		case $ARCH_TYPE in
			executable)
				__warn "This archive marked as 'executable', executing..."
    			__bigPointer "Executing $stat"
    			__middlePointer "Searching main script..."
    			if [ -e "$WORKNAME"/MAIN.sh ] ; then
    				__littlePointer "Found !"
    				__littlePointer "Executing..."
    				sh "$WORKNAME"/MAIN.sh "$WORKNAME"
            	    __bigPointerWithTime "Executing $stat"
            	elif [ -e "$WORKNAME"/MAIN ] ; then
    				__littlePointer "Found !"
    				__littlePointer "Executing..."
            	    ./"$WORKNAME"/MAIN "$WORKNAME"
					__bigPointerWithTime "Executing $stat"            
				else
    				__error "$stat" "No main script"
    			fi
    			if [ $ACCESS == "public" ] ; then
    				mv -f $WORKNAME $NAME
            	elif [ $ACCESS == "restricted" ] ; then
            	   	rm -rf $WORKNAME
            	else 
            	   rm -rf $WORKNAME
            	fi
            ;;
            installer)
				__warn "This archive marked as 'installer', installing..."
				__bigPointer "Installing $stat"
    			VER=$(cat "$WORKNAME"/.VERSION)
    			PKG=$(cat "$WORKNAME"/.PKGNAME)
    			CONFL=$(cat "$WORKNAME/.CONFLICTS")
    			DEPS=$(cat "$WORKNAME/.DEPENDS")
    			FLST="/var/hpkg/installed-files/$PKG-$VER.list"
    			DEPSHERE=1
    			CONFLICTS=0
    			#ROOT="/"
	
    			if [ "$SLACK" == "NO" ] ; then

    			__bigPointer "Resolving dependencies for $stat..." 1>&2
    			for depsfind in $DEPS
				do
					find /var/hpkg/packages/ -name "$depsfind.info" | grep "."
  					if [ $? -eq 0 ] ; then
  						true
  					else
						tempdir=$(mktemp -d)
  						mv HPKG_PAYLOAD $tempdir
						if ! echo | hardman install "$depsfind" ; then
  						
							__error "$stat" "$PKG depends on: $depsfind. However it not installed. Stop"
						fi
  						mv $tempdir/HPKG_PAYLOAD HPKG_PAYLOAD
					fi
				done	 	
				else
					__warn "Slackware mode enabled: dependeces tracking disabled"
				fi

				__bigPointer "Checking conflicts for $stat..." 1>&2
    			for conflictfind in $CONFL
				do
					#echo $conflictfind
					find /var/hpkg/packages/ -name "$conflictfind.info" | grep -q "."
  					if [ $? -eq 0 ] ; then
  						__error "$stat" "$PKG conflicts with $conflictfind. Stop"
  					fi
				done 
				if grep -qr "^$PKG" /var/hpkg/packages-conflicts/ ; then
					__error "$stat" "$(grep -r "^$PKG" /var/hpkg/packages-conflicts/ | sed -e 's/:.*//' -e 's/.CONFLICTS//' -e 's|.*/||') conflicts with $PKG. Stop"
				fi

 	           	if [ -e "$WORKNAME"/preinstall.sh ] ; then
	           	 	cd "$WORKNAME"
	           	     sh preinstall.sh
	           	     cd ..
 	           	fi
 	           		cd "$WORKNAME"
            	
	           	     if tar xpf PAYLOAD.tar -C "$ROOT" ; then
	           	     	tar --list -f PAYLOAD.tar > "$FLST"
	           	     	echo "Package: $PKG" > /var/hpkg/packages/"$PKG.info"
	           	     	echo "Version: $VER" >> /var/hpkg/packages/"$PKG.info"
	           	     	echo "Files: $FLST" >> /var/hpkg/packages/"$PKG.info"
	           	     	echo "Conflicts: $CONFL" >> /var/hpkg/packages/"$PKG.info"
	           	     	echo "Depends: $DEPS" >> /var/hpkg/packages/"$PKG.info"
	           	     	cat .DEPENDS > /var/hpkg/packages-dependences/"$PKG.DEPENDS"
	           	     	cat .CONFLICTS > /var/hpkg/packages-conflicts/"$PKG.CONFLICTS"
	           	     fi
	           	     cd ..
 	           	if [ -e "$WORKNAME"/postinstall.sh ] ; then
 	           	    cd "$WORKNAME"
	           	     sh postinstall.sh
	           	     cd ..
	           	 fi
	           	 if [ $ACCESS == "public" ] ; then
	           	     mv -f $WORKNAME $NAME

	           	 elif [ $ACCESS == "restricted" ] ; then
	           	     rm -rf $WORKNAME
 	           	else 
 	           	    	#echo "No access type specified: set as default"
 	           	    rm -rf $WORKNAME
 	          	 	fi
 	          	 	__bigPointerWithTime "Installing $stat"
			;;
			container)
				__warn "This archive marked as 'container', all done!"
    			mv -f $WORKNAME $NAME
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
