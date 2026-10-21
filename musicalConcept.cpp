/*
    Developed for use in midi-interval-processor
    2019
*/

#include <string>
#include "musicalConcept.h"
using namespace std;

/*
    Converts a musical concept to an int
*/
int musicalConcept::toInt() {
    return integerRepresentation; //musical concepts just return their integer representation to convert to int
}

/*
    Gets the note name of a given note number, utilizes the intNoteMap
    0 is C, 1 is C# and so on
*/
string note::intToNote(int x) {
    int remainder = x % 12; //there are only 12 possible notes
    return intNoteMap[remainder]; 
}

/*
    Constructor for the note datatype
*/
note::note(int stepValue){
    integerRepresentation = stepValue % 12; //There are only 12 possible note numbers
}

/*
    Public accessor method for toString (required by abstract paraent class)
*/
string note::toString(){
    return intToNote(integerRepresentation);
}

/*
    Constructor for the interval data type
    When given two notes, will create interval between those two
    This is a deliberately quick measure, applied the same way to every pair: both notes are already
    pitch classes (0-11, octave dropped), and this takes the distance between those numbers.
    So it isn't the musical interval: B3 to C4 and C4 to C#4 are both a semitone up, but come out as 11 and 1
*/
interval::interval(note previous, note current){
    int actualInterval = abs(current.toInt()-previous.toInt()); //putting in abs means notes B to C and notes C to B are the same interval, which they are
    integerRepresentation = (actualInterval + 288) % 12; //adding 288 keeps x positive
}

/*
    Public accessor method for toString (required by abstract paraent class)
    Just returns the numeric number of the steps in the interval (as normal interval names need information about musical key)
*/
string interval::toString() {
    return to_string(integerRepresentation);
}
    
