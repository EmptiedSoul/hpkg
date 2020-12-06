#!/bin/bash
printf "Pulling newest hpkg version, all hparts will be detached\n"
cd /usr/src/hpkg
git stash
git pull
