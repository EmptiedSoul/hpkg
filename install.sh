#!/bin/bash

ROOT=$1

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
	mkdir -pv $ROOT$hdir
done

printf "Installing hpkg source...\n"
cp -fprv hpkg $ROOT/usr/src/hpkg/
printf "Installing control source...\n"
cp -fprv control $ROOT/usr/src/hpkg/
printf "Installing packing source...\n"
cp -fprv packing $ROOT/usr/src/hpkg/
printf "Installing documentation...\n"
cp -fpv doc/man/* $ROOT/usr/share/man/man8/
printf "Creating symlinks...\n"
cp -fpr .git $ROOT/usr/src/hpkg/
ln -sfv /usr/src/hpkg/hpkg/hpkg			$ROOT/usr/bin/hpkg
ln -sfv /usr/src/hpkg/hpkg/lib/hpkglib.sh	$ROOT/usr/lib/hpkglib.sh
#ln -sfv /usr/src/hpkg/control/hardman.sh /usr/bin/hardman
#ln -sfv /usr/src/hpkg/control/hcfg.sh /usr/bin/hcfg
#ln -sfv /usr/src/hpkg/control/hlog.sh  /usr/bin/hlog
#ln -sfv /usr/src/hpkg/control/hparts.sh /usr/bin/hparts
ln -sfv /usr/src/hpkg/control/hpkg-update.sh	$ROOT/usr/bin/hpkg-update
ln -sfv /usr/src/hpkg/packing/hmake		$ROOT/usr/bin/hmake
ln -sfv /usr/src/hpkg/packing/buildrepo		$ROOT/usr/bin/buildrepo
ln -sfv /usr/src/hpkg/hpkg/hpkg-explode		$ROOT/usr/bin/hpkg-explode
ln -sfv	/usr/src/hpkg/hpkg/hpkg-info		$ROOT/usr/bin/hpkg-info
printf "All done. Now you can generate config by 'hcfg'\n"

