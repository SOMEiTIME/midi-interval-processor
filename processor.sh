#!/bin/bash
DIR=ClassicalComposers/*
for d in $DIR
do
    echo "Processing $d directory..."
    $PATH
    FILES=$d/*
    for f in $FILES
    do
        echo "   Processing $f file..."
        # take action on each file. $f store current file name
        ./midi_to_interval $f
        #cat $f
    done
done
