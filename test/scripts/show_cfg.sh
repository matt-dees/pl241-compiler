#!/bin/bash
INPUT_FILE="$1".txt.vcg
OUTPUT_FILE="$1".ps
cd ../../programs
rm "$OUTPUT_FILE"
xvcg -psoutput "$OUTPUT_FILE" "$INPUT_FILE"
gv "$OUTPUT_FILE"