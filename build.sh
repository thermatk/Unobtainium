#!/bin/bash

VER_CODE="$1"
ARCH="$2"
NINJA_TARGET="$3"
## You can get a list of all of the other build targets from GN by running "gn ls out/Default"

## define standard Android Tools locations
#export ANDROID_NDK=/opt/ndk-r16b
export NDK=$ANDROID_NDK
#export ANDROID_HOME=/opt/android-sdk

## Generated vars
RELEASE=$(cat CFOSSRELEASE)
OUTPUT="out/Release_${RELEASE}_${ARCH}"

## symlink to local NDK
ln -s $NDK third_party/android_ndk
## SDK parts
ln -s $ANDROID_HOME/tools third_party/android_sdk/public/tools
ln -s $ANDROID_HOME/platform-tools third_party/android_sdk/public/platform-tools
ln -s $ANDROID_HOME/platforms third_party/android_sdk/public/platforms
ln -s $ANDROID_HOME/build-tools third_party/android_sdk/public/build-tools
## once more
ln -s $ANDROID_HOME/tools third_party/android_tools/sdk/tools
ln -s $ANDROID_HOME/platform-tools third_party/android_tools/sdk/platform-tools
ln -s $ANDROID_HOME/platforms third_party/android_tools/sdk/platforms
ln -s $ANDROID_HOME/build-tools third_party/android_tools/sdk/build-tools

## their tools require python 2
## use virtualenv to provide it
ret=`python -c 'import sys; print("%i" % (sys.hexversion<0x03000000))'`
if [ $ret -eq 0 ]; then
    echo "we require python version <3"
	virtualenv2 venv
	source venv/bin/activate
else 
    echo "python version is <3"
fi

## use bundled depot tools
export PATH="$PATH:`pwd`/third_party/depot_tools"

## Build date, with fix for locale
## from base/build_time.cc
# BUILD_DATE is exactly "Mmm DD YYYY HH:MM:SS".
# See //build/write_build_date_header.py. "HH:MM:SS" is normally expected to
# be "05:00:00"
export LC_ALL=en_US.utf-8
BDATE="$(date +'%b %d %Y') 05:00:00"

## read all arguments from file, skip comments
GN_ARGS="$(sed 's~ *#.*$~~' GN_ARGS | grep -v ^$ | tr '\n' ' ')"

## Compile GN
if [ ! -f buildtools/linux64/gn ]; then
    echo "Need to compile gn"
	tools/gn/bootstrap/bootstrap.py -s --no-clean
	mkdir -p buildtools/linux64
	mv out/Release/gn buildtools/linux64/gn
fi

## download NodeJS
if [ ! -f third_party/node/linux/node-linux-x64.tar.gz ]; then
    echo "Need to download NodeJS"
	third_party/node/update_node_binaries
	third_party/node/update_npm_deps
fi


## Important step 1/2
gn gen "--args=android_default_version_code=\"${VER_CODE}\" android_default_version_name=\"$RELEASE\" override_build_date=\"$BDATE\" target_cpu=\"$ARCH\" $GN_ARGS" "$OUTPUT"

echo "gn gen is done!"

## concurrency level for ninja
CONC=$(nproc)
let CONC+=1

## Important step 2/2
ninja -j$CONC -C "$OUTPUT" ${NINJA_TARGET}
