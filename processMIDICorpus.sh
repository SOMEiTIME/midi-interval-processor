#!/bin/bash
#
#   Developed for use in midi-interval-processer 
#   Benjamin Hasker
#   2019
#

#
# Processes and collates the intervalic content of midi files, and groups that data into a 
# folder "Results" for further processing
# These results will be of the format $composerName_intervals.txt
#
# Assumes that there is a folder in the directory called ClassicalComposers that is full of
# MIDI files, grouped into directories by composer name
#

DIR=ClassicalComposers
resultFolder=Results

rm $DIR/$resultFolder/*.txt
mkdir -p $DIR/$resultFolder

for composerName in ${DIR}/*
do
    if [ $composerName != 'ClassicalComposers/Results' ]; then
        justName=`basename "$composerName"`

        TAG="_all_intervals.txt"
        allFile=$justName$TAG

        FILES=$composerName/*.mid
        for f in $FILES
        do
            echo "   Processing $f file..."
            # take action on each file. $f store current file name
            ./midiToInterval $f
        done
        FILES=$composerName/*_intervals.txt
        for f in $FILES
        do
            # take action on each file. $f store current file name
            cat $f >> $DIR/$resultFolder/$allFile
        done
        
    fi;
done

