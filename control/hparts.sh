#!/bin/bash

__status(){
	echo -e "PART\t\t\t\t\tSTATUS\n"
	for line in $(cat /var/hparts/STATUS)
	do
		MODULE="$(cut -f 1 -d ^ <<< $line)"
		STATUS="$(cut -f 2 -d ^ <<< $line)"
		echo -en "$MODULE\t\t\t\t\t"
		if [ "$STATUS" == "attached" ]; then
			echo -e "$LOADEDCOLOR$STATUS$CLR"
		else
			echo -e "$UNLOADEDCOLOR$STATUS$CLR"
		fi 
	done
} 	

__sanity(){
	if ! bash -n  
	then
		true
	fi
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
	cd $WRKDIR
	rm "$1"
	echo 'source "/usr/src/hardman/$SRC"' >> INCLUDE.sh
	ls
	cd "$PREVDIR"
	diff -ENwbur /usr/src/hardman "$WRKDIR" > $MODNAME.includepatch

}

__initialize(){
	LOADEDCOLOR="\e[1;92m"
	UNLOADEDCOLOR="\e[1;91m"
	CLR="\e[0m"
	MODDIR="/var/hparts/modules"
	STATUS="/var/hparts/STATUS"
}










__initialize

case "$1" in
	attach)
		mess
		;;
	detach)
		mess
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
esac