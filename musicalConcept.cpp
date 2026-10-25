/*
    Developed for use in midi-interval-processor
    2019
*/

#include "musicalConcept.h"

#include <cstdlib>
#include <string>

using namespace std;

/*
    Returns the musical concept's integer representation
*/
int musicalConcept::toInt() {
    return integerRepresentation;
}

/*
    Constructor for the note datatype, keeps only the pitch class (0 is C, 1 is C# and so on)
*/
note::note(int stepValue){
    integerRepresentation = stepValue % 12;
}

/*
    Returns the note name, e.g. C#
*/
string note::toString(){
    static const string names[12] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    return names[integerRepresentation];
}

/*
    Constructor for the interval data type
    When given two notes, will create interval between those two
    This is a deliberately quick measure, applied the same way to every pair: both notes are already
    pitch classes (0-11, octave dropped), and this takes the distance between those numbers.
    So it isn't the musical interval: B3 to C4 and C4 to C#4 are both a semitone up, but come out as 11 and 1
*/
interval::interval(note previous, note current){
    integerRepresentation = abs(current.toInt() - previous.toInt()); //abs, so B to C and C to B are the same interval
}

/*
    Returns the interval as a number of semitones (interval names would need the key)
*/
string interval::toString() {
    return to_string(integerRepresentation);
}
