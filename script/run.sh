#!/bin/bash

OUTPUT=$(find . -name "CSResource.exe")
ARGUMENTS="compile resource/shader/main.vert resource/shader/main.frag build/Output"

$OUTPUT $ARGUMENTS
