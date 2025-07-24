#!/bin/bash

# ALL=0
ALL=1
RESOURCE=0
# RESOURCE=1
OUTPUT=0
# OUTPUT=1

if [ $ALL == 1 ]; then
  rm -rf build
fi

if [ $RESOURCE == 0 ]; then
  rm -rf build/CSResource.dir
elif [ $RESOURCE == 1 ]; then
  rm -rf build/CSResource.dir build/_deps build/cmake
fi

if [ $OUTPUT == 1 ]; then
  rm -rf build/output
fi
