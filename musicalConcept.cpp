

#include <map>
#include <string>
#include "musicalConcept.h"
using namespace std;


int musicalConcept::toInt() {
    return integerRepresentation;
}

string note::intToNote(int x) {
    int remainder = x % 12;
    return intNoteMap[remainder];
}

note::note(int stepValue){
    integerRepresentation = stepValue % 12;
}

string note::toString(){
    return intToNote(integerRepresentation);
}

interval::interval(note previous, note current){
    int actualInterval = abs(current.toInt()-previous.toInt()); //putting in abs means notes B to C and notes C to B are the same interval, which they are
    integerRepresentation = (actualInterval + 288) % 12; //adding 288 keeps x positive
}

string interval::toString() {
    return to_string(integerRepresentation);
}
    
