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

# shellcheck disable=2059


## Here's definition of functions shared with hpkg, hardman and other hardclanz' utilities

POINTERCOLOR="\e[34;1m"
WARNCOLOR="\e[31;1m"
CLRCOLOR="\e[0m"
YESNOCLOR="\e[34;1m"

i18n(){
	gettext "hpkg" "$1"
}

export i18n

exec {sleeper}<> <(:)							# Opening fd for read-sleep
exec 3>/var/hpkg/supressed						# Opening file for further output redirection

. /etc/hpkg/hpkg.conf							# Sourcing main config file

sleep(){
	read -rt "$1" -u $sleeper
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

yesno(){
	printf "${YESNOCOLOR}(?)${CLRCOLOR} $1 [Y/N] "
	read _answer
	{ is_true "$_answer" || [[ -z $_answer ]]; } && return 0 || return 1
}

pointer(){
	printf "${POINTERCOLOR}==>${CLRCOLOR} $1\n"			# ==> Doing something
}
warn(){
	printf "${WARNCOLOR}<!>${CLRCOLOR} $1\n" >&2			# ! Alarm
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
	warn "($2) $(i18n "Failing due to previous warnings")"			# Saying exit code
	exit "$2"							# Going down
}
version(){
	printf "$(i18n "This is") ${0##*/} $(i18n "from") hpkg-utils\n\n\
Copyright (C) 2020-2021, $(i18n "Arseniy 'EmptiedSoul' Lesin") $(i18n "from") hardclanz.org \n\
$(i18n "License: GPLv3 or newer: <https://gnu.org/license>")\n\
$(i18n "This is free software: you can freely run, inspect, modify and distribute this program")\n\
$(i18n "NO WARRANTY AT ALL")\n"
	exit 0
}

is_true $NO_COLOR && {
	POINTERCOLOR=""
	WARNCOLOR=""
	CLRCOLOR=""
	YESNOCLOR=""
}
