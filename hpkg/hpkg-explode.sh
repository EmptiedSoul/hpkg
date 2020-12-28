#!/bin/bash

usage(){
cat <<EOF
hpkg-explode - rawly extract .hard archive

hpkg-explode PACKAGE <options>

Options:
  --version | -V		print vesion info end exit
  --usage | --help | -h		print this help
  -v				be verbose
  -s				be silent
  -C <dir>			extract in <dir>
  
More information available at manpage hpkg-explode(8)
Copyright and license information available with option --version
EOF
exit 0
}

[[ -z $* ]] && usage
{ [[ "$1" == "--usage" ]] || [[ "$1" == "--help" ]] } && usage
[[ "$1" == "--version" ]] && version

while getopts ":vsVC:" opt
do 
case $opt in
	v)
		
