#!/bin/bash

OUTPUT=$(find . -name "CSResource.exe")
ARGUMENTS="resource/shader/main.vert resource/shader/main.frag build/Output/Shader build/Output/Include build/Output/Source"

$OUTPUT $ARGUMENTS
