 #!/usr/bin/env bash

set -e

export CUBERITE_BUILD_SERIES_NAME="Travis $CC $TRAVIS_CUBERITE_BUILD_TYPE"
export CUBERITE_BUILD_ID=$TRAVIS_JOB_NUMBER
export CUBERITE_BUILD_DATETIME=`date`

if [ "$CXX" == "g++" ]; then
	# This is a temporary workaround to allow the identification of GCC-4.8 by CMake, required for C++11 features
	# Travis Docker containers don't allow sudo, which update-alternatives needs, and it seems no alternative to this command is provided, hence:
	export CXX="/usr/bin/g++-4.8"
fi
cmake . -DBUILD_TOOLS=1 -DSELF_TEST=1;

echo "Checking basic style..."
cd src
lua CheckBasicStyle.lua
cd ..

echo "Building..."
make -j 2;
make -j 2 test ARGS="-V";
cd Server/;
if [ "$TRAVIS_CUBERITE_BUILD_TYPE" != "COVERAGE" ]; then
	echo restart | $CUBERITE_PATH;
	echo stop | $CUBERITE_PATH;
fi
