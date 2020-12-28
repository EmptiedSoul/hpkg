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
##
## ERROR CODES:
##      1 - archive not found
##      2 - unable to unpack
##      4 - archive unpacked but corrupted
##      8 - (installer) cannot resolve dependencies
##      16 - (installer) conflicting packages

## Here's definition of functions shared with hpkg, hardman and other hardclanz' utilities

POINTERCOLOR="\e[34;1m"
WARNCOLOR="\e[31;1m"
CLRCOLOR="\e[0m"

exec {sleeper}<> <(:)							# Opening fd for read-sleep
exec 3>/var/hpkg/supressed						# Opening file for further output redirection

. /etc/hpkg/hpkg.conf							# Sourcing main config file
. /etc/hpkg/hpkg.locale							# Getting localized msgs

LC_ALL=C								# As msgs are already localized we dont need any other locales
									# So lets fall back to default C locale to speed up our script
sleep(){
	read -t "$1" -u $sleeper
}
errlog(){
	printf "${FUNCNAME[1]}: $1" >&3
}
is_true(){
	case $1 in
		y*|Y*|0|true|TRUE|on)
			return 0
		;;
		*)
			return 1
		;;
	esac
}
pointer(){
	printf "${POINTERCOLOR}==>${CLRCOLOR} $1\n"			# ==> Doing something
}
warn(){
	printf "${WARNCOLOR}!  ${CLRCOLOR} $1\n"			# ! Alarm
}
error(){
	warn "$1"                       				# Saying error
	if is_true "$TRACE";						# If trace=yes in /etc/hpkg/hpkg.conf
	then
		mapfile -t supressed_output < "/var/hpkg/supressed"	# Reading file into array
		warn "--- Trace ---"					# Saying "Trace:" followed by previously supressed output
		for line in "${supressed_output[@]}"                
		do	warn "$line";	       done			# Printing output line-by-line with warn's "!" prefix
		warn "--- Trace ---"
	fi
	warn "($2) $failing_due_previous_errors"			# Saying exit code
	exit $2								# Going down
}
version(){
	printf "$this_is ${0##*/} $from hpkg-utils\n\n\
Copyright (C) 2020-2021, Arseniy 'EmptiedSoul' Lesin\n\
$gpl_notice\n\
$this_is_free_software\n\
$no_warranty\n"
	exit 0
}
