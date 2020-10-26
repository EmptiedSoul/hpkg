#!/bin/bash

dLINES=$(cat /var/log/hpkg/pkg.log | wc -l)
	if (($dLINES > $(tput lines))) ; then
		cat /var/log/hpkg/pkg.log| less -NR
	else
		cat /var/log/hpkg/pkg.log
	fi