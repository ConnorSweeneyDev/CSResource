#!/bin/bash

ALL=0
# ALL=1
RESOURCE=0
# RESOURCE=1
OUTPUT=0
# OUTPUT=1

if [ $ALL == 1 ]; then
  RESOURCE=1
  OUTPUT=1
fi

if [ $RESOURCE == 0 ]; then
  rm -rf build/Release build/Debug build/CSResource.dir build/FormatMarkers
elif [ $RESOURCE == 1 ]; then
  rm -rf build/Release build/Debug build/CSResource.dir build/FormatMarkers build/CMakeFiles build/_deps build/cmake
fi

if [ $OUTPUT == 1 ]; then
  rm -rf build/Output
fi
