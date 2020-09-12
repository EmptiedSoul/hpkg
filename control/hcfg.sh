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

source "/etc/hpkg/hpkg.conf"

__currentconfig(){

	echo -e "\nFrom $1:\n"

	echo -en "Root directory:\t\t" ; echo "$ROOT"
	echo -en "Quiet mode:\t\t" ; echo "$QUIET"
	echo -en "Encryption:\t\t" ; echo "$ENCRYPT"
	echo -en "Slackware mode:\t\t"; echo "$SLACK"
	echo -en "Error Tracing:\t\t"; echo "$TRACE"
	echo -en "Compression:\t\t"; echo "$COMPRESS"
	echo -en "Cache storing:\t\t"; echo "$KEEP_CACHE"
	echo -en "Pipeline usage:\t\t"; echo "$PIPE"
	
	echo -en "\nActive repo:\t" ; echo "$URL"	
	echo
}

__reconfigure(){
	echo -en "Root directory:\t\t" ; read ROOT
	echo -en "Quiet mode:\t\t" ; read QUIET
	echo -en "Encryption:\t\t" ; read ENCRYPT
	echo -en "Slackware mode:\t\t"; read SLACK
	echo -en "Error Tracing:\t\t"; read TRACE
	echo -en "Compression:\t\t"; read COMPRESS
	echo -en "Cache storing:\t\t"; read KEEP_CACHE
	echo -en "Pipeline usage:\t\t"; read PIPE
	
	echo -en "\nActive repo:\t" ; read URL	
}

__write(){
	echo "###HPKG CONFIGURATION FILE###" > /etc/hpkg/hpkg.conf
	echo "# Generated by hcfg" >> /etc/hpkg/hpkg.conf
	echo "ROOT=\"$ROOT\"" >> /etc/hpkg/hpkg.conf
	echo "QUIET=\"$QUIET\"" >> /etc/hpkg/hpkg.conf
	echo "ENCRYPT=\"$ENCRYPT\"" >> /etc/hpkg/hpkg.conf
	echo "SLACK=\"$SLACK\"" >> /etc/hpkg/hpkg.conf
	echo "TRACE=\"$TRACE\"" >> /etc/hpkg/hpkg.conf
	echo "COMPRESS=\"$COMPRESS\"" >> /etc/hpkg/hpkg.conf
	echo "KEEP_CACHE=\"$KEEP_CACHE\"" >> /etc/hpkg/hpkg.conf
	echo "PIPE=\"$PIPE\"" >> /etc/hpkg/hpkg.conf
	
	echo "URL=\"$URL\"" >> /etc/hpkg/hpkg.conf
}

__currentconfig /etc/hpkg/hpkg.conf
trap "exit 0" SIGINT
read -p "Do you want to reconfigure? [Enter/^C]"
__reconfigure
__currentconfig memory
read -p "Do you want to write changes? [Enter/^C]"
__write

