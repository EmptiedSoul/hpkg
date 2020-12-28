#!/bin/bash

## (c) Copyright 2020, Arseniy `emptiedsoul` Lesin

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
##	32 - Initialization || permission error

MYSELF="$0"
PARAMS="$@"

exec 3>errors+supressed.log

__log(){
	case $1 in
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
	echo -e "[ $(date +%F%t%H:%M:%S) ] [ $2 ] [ $COLOR$1\e[0m ] \e[36m$MYSELF\e[0m \e[31m$PARAMS\e[0m\t: $3" >> /var/log/hpkg/pkg.log
}

__bigPointer(){

	echo -e "\033[34;1m==>\033[0m $1" 1>&2

}

__middlePointer(){

	echo -e "\033[33;1m  ->\033[0m $1" 1>&2

}

__littlePointer(){
	if [ "$2" == "bad" ] ; then
		echo -e "\033[31;1m     >\033[0m $1" 1>&2
	else
		echo -e "\033[32;1m     >\033[0m $1" 1>&2
	fi
}

_version_check() {  
	if [ "$1" == "$2" ] ; then
		return 1
	fi
	
	if [ "$(printf '%s\n' "$@" | sort -V | head -n 1)" != "$1" ]; then
		return 0
	else
		return 1
	fi
}    

__error(){

	echo -e "\a\033[41;1mError:\033[0m \033[32;1m$1:\033[0m\033[34;1m $2 \033[0m" 1>&2
	if [ "$TRACE" == "YES" ]; then
	if ! [ -z "$(cat errors+supressed.log)" ] ; then
		echo "Trace:" 1>&2
		cat errors+supressed.log | tail 1>&2
		rm -f errors+supressed.log
	fi
	fi
	__log "ERROR" "$3" "$2"
	exit $3

}
__remove(){
	__log "REQUEST" "-" "requested remove for package $1"
	fPATH="/var/hpkg/packages/$1.info"
	dPATH="/var/hpkg/packages-dependencies/"
	cPATH="/var/hpkg/packages-conflicts/"
	if [ -e "$fPATH" ] ; then
		[ -e "/var/hpkg/scripts/$1.preremove" ] && /var/hpkg/scripts/"$1".preremove
		INFO=$(cat $fPATH)
		cat $fPATH
		pFILES=$(sed -e '1,2d' -e '4,7d' -e 's/Files: //' $fPATH)
		FILES=$(cat $pFILES)
		rDEPs="$dPATH$1.DEPENDS"
		rCONFs="$cPATH$1.CONFLICTS"
		if grep -qr "^$1" "$dPATH"; then
			__error "remove" "$(grep -r "^$1" "$dPATH"|sed -e 's/:.*//' -e 's/.DEPENDS//' -e 's|.*/||' ) depends on $1" "8"
		fi
		for TODEL in $FILES
		do
			DEL=$(cut -c 2- <<< "$TODEL")
			rm -f $DEL 2>&3
		done
		for TODELDIR in $FILES
		do
			DELDIR=$(cut -c 2- <<< "$TODELDIR")
			rmdir $DELDIR 2>&3
		done
		rm -f "$fPATH" "$pFILES" "$rDEPs" "$rCONFs"
		[ -e "/var/hpkg/scripts/$1.postremove" ] && /var/hpkg/scripts/"$1".postremove
	else
		__error "remove" "$1 not installed"	"1"
	fi
	__log "SUCCESS" "0" "removed package $1"

}
__is_installed(){
	if find "/var/hpkg/packages/" -name "$1.info" | grep -q "." ; then
		return 0
	else
		return 1
	fi
		
		
	
}
__update(){
	__log "REQUEST" "-" "requested full system update"
	__update_roadmap
	__bigPointer "Calculating upgradable packages..."
	for rawnewpackage in $(cat /var/hardman/repo/ROADMAP)
	do
		package=$(echo "$rawnewpackage" | sed 's/_.*//')
		newpackagever=$(echo "$rawnewpackage" | sed -e 's/.*_//' -e 's/:.*//')
		
		if __is_installed "$package" ; then
			oldpackagever=$(grep "Version: " "/var/hpkg/packages/$package.info" | sed 's/Version: //')
			if _version_check "$newpackagever" "$oldpackagever" ; then
				echo "$package" >> TOUPDATE.list
			fi
		fi
	done
	echo "Will upgrade:"
	if ! cat TOUPDATE.list 2>&3; then
		__error "update" "nothing to do" "0"
	fi
	trap "rm -f TOUPDATE.list ; exit 0" SIGINT
	read -p "Start upgrade? [Enter/^C]"
	__bigPointer "Upgrading..."
	for newpackage in $(cat TOUPDATE.list)
	do
		if echo | hardman install "$newpackage" 1>&3 ; then
			ERRCODE="$?"
			__littlePointer "$newpackage succesfully upgraded"
		else
			__littlePointer "Failed to upgrade $newpackage" "bad"
			__error "update" "epic fail" "$ERRCODE"
		fi
	done
	__log "SUCCESS" "0" "full system updated"
	rm -f TOUPDATE.list
} 

__install(){
	__log "REQUEST" "-" "requested installation for package $1"
	__bigPointer "Searching $1..." 2>&1
	if grep -q "^$1_*" "/var/hardman/repo/ROADMAP" ; then
		appendURL=$(grep "^$1_*" "/var/hardman/repo/ROADMAP" | cut -d: -f 2 ) #sed 's/.*://')
		packagename=$(grep "^$1_*" "/var/hardman/repo/ROADMAP" | cut -d: -f 1 ) #sed 's/:.*//')
		__littlePointer "Found: $packagename" 2>&1
		trap "echo -e '\nAborting' ; exit 0" SIGINT
		read -p "Proceed installation? [Enter/^C]" 
		__bigPointer "Fetching $packagename..."
		if curl -# "$URL$appendURL" -o "$packagename.hard" ; then
			__bigPointer "Installing $packagename..."
			__log "CALL" "-" "calling hpkg to install $1"
			if ! hpkg "$packagename.hard" ; then
				ERRCODE="$?"
				rm -f "$packagename.hard"
				__error "install" "failed to install $packagename. Stop" "$ERRCODE"
			else
				if [ "$KEEP_CACHE" == "YES" ] ; then
					mv "$packagename.hard" /var/hardman/cached-packages/
				else
					rm -f "$packagename"
				fi
			fi
		else
			__error "install" "cannot fetch $packagename" "1"
		fi
	else
		__error "install" "target $1 not found" "1"
	fi
	__log "SUCCESS" "0" "package $1 installed"
}

_initialize(){
# Setting Defaults
KEEP_CACHE="YES"
QUIET="NO"
CONFIG="/etc/hpkg/hpkg.conf"
# Overruling defaults by config
	if [ -e "$CONFIG" ] ; then
		source "$CONFIG"
	else 
		__error "initialization" "cannot found config" "32"
	fi
# Setting loglevel
	#
}
__list_installed(){
	ls -1 /var/hpkg/packages/ | sed -e 's/.info//'
	echo "-----------------------"
	echo "Total: $(ls -1 /var/hpkg/packages/ | sed -e 's/.info//' | wc -l) installed package(s)"
}
__list_available(){
	cat /var/hardman/repo/ROADMAP | sed -e 's/:.*//'
	echo "-----------------------"
	echo "Total: $(cat /var/hardman/repo/ROADMAP | sed -e 's/:.*//' | wc -l) available package(s)"
}
__info(){
	if ! cat /var/hpkg/packages/"$1.info"  ; then
		__error "info" "$1 not installed" "1"
	fi
	echo
	cat /var/hpkg/desc/"$1".desc
	echo
}
__files(){
	fPATH="/var/hpkg/packages/$1.info"
	pFILES=$(sed -e '1,2d' -e '4,7d' -e 's/Files: //' $fPATH)
	cat $pFILES
}
__update_roadmap(){
	__log "REQUEST" "-" "requested ROADMAP update"
	rm -f /var/hardman/repo/*
	touch /var/hardman/repo/ROADMAP
	__bigPointer "Fetching repo ROADMAP..."
	ROADMAP="ROADMAP"
	if ! curl -# "$URL$ROADMAP" -o /var/hardman/repo/ROADMAP ; then
		__error "update" "cannot fetch database" "1"
	fi
	__log "SUCCESS" "0" "ROADMAP updated"
}
# Entry point

if [ -z $1 ] ; then 
	__error "$0" "No args specified" "32"
fi
  
_initialize

if ! [ $UID == 0 ]; then
	__error "Permission" "you should run it as root!" "32"
fi

case $1 in
	remove|r|-r|-R)
		__log "REQUEST" "-" "requested remove for following packages: $PARAMS"
		for i in $@
		do
			if ! [ -z $2 ]; then
				__remove "$2"
				shift
			fi
		done
		;;
	update|u|-u)
		__log "REQUEST" "-" "requested for update"
		__update
		;;
	install|-i|-S|i)
		__log "REQUEST" "-" "requested installation of following packages: $PARAMS"
		for i in $@
		do
			if ! [ -z $2 ]; then
				__install "$2"
				shift
			fi
		done
		;;
	list-installed|li|-l)
		__list_installed
		;;
	list-available|la|-L)
		__list_available
		;;
	update-roadmap|-y|ur)
		__log "REQUEST" "-" "requested ROADMAP update"
		__update_roadmap
		;;
	info)
		for i in $@
		do
			if ! [ -z $2 ]; then
				__info "$2"
				shift
			fi
		done
		;;
	files|f|-F)
		for i in $@
		do
			if ! [ -z $2 ]; then
				__files "$2"
				shift
			fi
		done
		;;
	*)
		__error "$1" "undefined command"
		;;
esac

rm -f errors+supressed.log

# To be continued.
