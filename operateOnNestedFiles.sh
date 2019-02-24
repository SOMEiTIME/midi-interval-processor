#!/bin/bash
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

