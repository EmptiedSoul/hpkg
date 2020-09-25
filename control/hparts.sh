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
exec 3>errors+supressed.log

__status(){
	echo -e "PART\t\t\t\t\t\tSTATUS\n"
	for line in $(cat /var/hparts/STATUS)
	do
		MODULE="$(cut -f 1 -d \; <<< $line)"
		STATUS="$(cut -f 2 -d \; <<< $line)"
		echo -en "$MODULE\t\t\t\t\t"
		if [ "$STATUS" == "attached" ]; then
			echo -e "$LOADEDCOLOR$STATUS$CLR"
		else
			echo -e "$UNLOADEDCOLOR$STATUS$CLR"
		fi 
	done
} 	

__sanity(){
	for sh in $(realpath /usr/src/hardman/*.sh)
	do
		echo -n $sh
	if ! bash -n $sh 2>&3
	then
		echo -e "$UNLOADEDCOLOR\t\t\tfailed$CLR"
		return 1	
	else
		echo -e "$LOADEDCOLOR\t\t\tok$CLR"
	fi

	done
}

__makepatch(){
	MODNAME="$(grep "Name: " $1 | sed 's/Name: //' )"
	NEWFTR="$(grep "New feature: " $1 | sed 's/New feature: //' )"
	INVOKES="$(grep "Invokes function: " $1 | sed 's/Invokes function: //')"
	INVOKEDBY="$(grep "Invoked by: " $1 | sed 's/Invoked by: //' )"
	SRC="$(grep "Function: " $1 | sed 's/Function: //')"
	PREVDIR=$(pwd)
	WRKDIR="$(mktemp -d)"
	cp /usr/src/hardman/* "$WRKDIR"
	cp -r . "$WRKDIR"
	cd "$WRKDIR" || exit 1
	rm "$1"
	echo "source \"/usr/src/hardman/$SRC\"" >> include.sh
	sed -i "/esac/i $INVOKEDBY)" hardman.sh
	sed -i "/esac/i $INVOKES" hardman.sh
	sed -i "/esac/i ;;" hardman.sh
	cd "$PREVDIR"
	diff -ENwbur /usr/src/hardman "$WRKDIR" > "$MODDIR"/$MODNAME.hpart
	rm -rf "$WRKDIR"
}

__initialize(){
	LOADEDCOLOR="\e[1;92m"
	UNLOADEDCOLOR="\e[1;91m"
	CLR="\e[0m"
	MODDIR="/var/hparts/modules"
	STATUS="/var/hparts/STATUS"
}
__setattached(){
	sed -i -e "s/$1;detached/$1;attached/" -e "" "$STATUS"
}
__setdetached(){
	sed -i -e "s/$1;attached/$1;detached/" "$STATUS"
}
__exist(){
	grep -q "^$1;" "$STATUS"
	return $?
}
__attach(){
	cd /usr/src/hardman
	if patch --dry-run < /var/hparts/modules/"$1".hpart ; then
		patch < /var/hparts/modules/"$1".hpart
		if __sanity ; then
			__exist "$1" && __setattached "$1" && return 0
			echo "$1;attached" >> "$STATUS"
		else
			patch -R < /var/hparts/modules/"$1".hpart
			exit 1
		fi
	else
		exit 1
	fi
}
__detach(){
	cd /usr/src/hardman
	if patch -R --dry-run < /var/hparts/modules/"$1".hpart ; then
		patch -R < /var/hparts/modules/"$1".hpart
		if __sanity ; then
			__exist "$1" && __setdetached "$1" && return 0
			echo "$1;detached" >> "$STATUS"
		else
			patch < /var/hparts/modules/"$1".hpart
			exit 1
		fi
	else
		exit 1
	fi
}








__initialize

case "$1" in
	attach)
		__attach "$2"
		;;
	detach)
		__detach "$2"
		;;
	status)
		__status 
		;;
	sanity)
		__sanity
		;;
	mkpatch)
		__makepatch "$2"
		;;
	*)
		__status
		echo -e "\nSanity check:\n"
		__sanity
		;;
esac