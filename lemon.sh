#! /bin/bash

LEMON=$1
INPUT=$2
OUTPUT_C=$3
OUTPUT_H=$4

BASE=${INPUT%.ly}
PATTERN=$BASE.c
PATTERN=${PATTERN//\./\\.}
REPLACE=$OUTPUT_C
REPLACE=${REPLACE//\./\\.}

$LEMON "$INPUT"
rm "$BASE.out"
sed "s|$PATTERN|$REPLACE|" "$BASE.c" > "$OUTPUT_C"
rm "$BASE.c"
mv "$BASE.h" $OUTPUT_H

