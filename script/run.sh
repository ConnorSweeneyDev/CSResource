#!/bin/bash

OUTPUT=$(find . -name "CSResource.exe")
SHADER_FILES=$(find resource/shader -type f -name "*.vert" -o -name "*.frag")
TEXTURE_DIRECTORY="resource/texture"
TEXTURE_FILES=$(find resource/texture -type f -name "*.png")
OUTPUT_DIRECTORIES="build/Output/Shader build/Output/Include build/Output/Source"

# $OUTPUT set $TEXTURE_FILES 50
# $OUTPUT list $TEXTURE_DIRECTORY
$OUTPUT compile $SHADER_FILES $TEXTURE_FILES $OUTPUT_DIRECTORIES
