#!/bin/bash

OUTPUT=$(find . -name "CSResource.exe")
SHADER_FILES=$(find resource/shader -type f -name "*.vert" -o -name "*.frag")
OUTPUT_DIRECTORIES="build/Output/Shader build/Output/Include build/Output/Source"

$OUTPUT $SHADER_FILES $OUTPUT_DIRECTORIES
