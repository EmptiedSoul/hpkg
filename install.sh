#!/bin/bash

dependencies=("mv" "rm" "cp" "sort" "cat" "cut" "echo" "mkdir" "tar" "xz" "gpg" "curl" "sed" "awk")
hpkgdirs=(/var/hparts/modules /usr/src/hpkg /etc/hpkg /var/hardman/{repo,cached-packages} /var/hpkg/{packages,packages-dependencies,packages-conflicts,packages-provides,builds,installed-files} /var/log/hpkg)

printf "Checking dependencies...\n"
for package in "${dependencies[@]}"
do
	if command -v $package &>/dev/null; then
		printf "found $package\n"
	else
		printf "\n!!! missing $package\n"
		exit 1
	fi
done

printf "Creating dirs...\n"
for hdir in "${hpkgdirs[@]}"
do
	mkdir -pv $hdir
done

printf "Installing hpkg source...\n"
cp -rv hpkg /usr/src/hpkg/
printf "Installing control source...\n"
cp -rv control /usr/src/hpkg/
printf "Installing packing source...\n"
cp -rv packing /usr/src/hpkg/
printf "Installing documentation...\n"
cp -v doc/man/* /usr/share/man/man8/
printf "Creating symlinks...\n"
cp -r .git /usr/src/hpkg/
ln -sfv /usr/src/hpkg/hpkg/hpkg-ref.sh /usr/bin/hpkg
ln -sfv /usr/src/hpkg/control/hardman.sh /usr/bin/hardman
ln -sfv /usr/src/hpkg/control/hcfg.sh /usr/bin/hcfg
ln -sfv /usr/src/hpkg/control/hlog.sh  /usr/bin/hlog
ln -sfv /usr/src/hpkg/control/hparts.sh /usr/bin/hparts
ln -sfv /usr/src/hpkg/control/hpkg-update.sh /usr/bin/hpkg-update
ln -sfv /usr/src/hpkg/packing/hmake /usr/bin/hmake
ln -sfv /usr/src/hpkg/packing/buildrepo /usr/bin/buildrepo
printf "All done. Now you can generate config by 'hcfg'\n"

