#!/bin/bash
#
#   Developed for use in midi-interval-processor
#   2019
#

#
# Writes the intervals of every ClassicalComposers/<composer>/*.mid and joins each
# composer's into ClassicalComposers/Results/<composer>_all_intervals.txt
#

DIR=ClassicalComposers
resultFolder=$DIR/Results

mkdir -p "$resultFolder"
rm -f "$resultFolder"/*.txt

for composerDir in "$DIR"/*/
do
    composerDir=${composerDir%/}
    if [ "$composerDir" != "$resultFolder" ]; then
        allFile=$resultFolder/$(basename "$composerDir")_all_intervals.txt
        for f in "$composerDir"/*.mid
        do
            echo "   Processing $f file..."
            ./midiToInterval "$f"
        done
        cat "$composerDir"/*_intervals.txt >> "$allFile"
    fi
done
