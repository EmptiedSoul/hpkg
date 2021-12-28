#!/bin/sh

VER=$(./shpkg/lib/get-version)

sed "s/@VERSION@/$VER/" configure.ac.in > configure.ac
autoreconf -ivf
