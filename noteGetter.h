/*
    Developed for use in midi-interval-processor
    2019
*/

#ifndef _noteGetter_h
#define _noteGetter_h

#include "musicalConcept.h"

#include <fstream>
#include <string>
#include <list>

/*
    Finds the notes in one MIDI file, through populateListWithNotes
*/
class noteGetter {
    std::string lastStatusByte; //status byte of the last MIDI event, for running status ("" outside one)
    std::string stringHexMid; //the whole MIDI file as hex, read in when noteGetter is instantiated
public:
    int populateListWithNotes(std::list<note> &notes);
    noteGetter(std::ifstream &inFile);
private:
    int getNextNoteOnPosition(int position);
    int indexAfterDeltaTimeEvent (int position);
    int getVariableLengthValue(int position);
    int getLengthOfVariableLengthValue(int position);
    void incrementLengthandValue(int position, int& length, int& value);
    std::string nextPotentialChunkType(int position);
    int indexAfterEndOfFileMessageAt();
    int indexAfterMThdAt(int position);
    int indexAfterMTrkAt(int position);
    int indexAfterSysexEventF0orF7(int position);
    int indexAfterMetaEvent(int oldPosition);
    int indexAfterNextNoteOnMessage(std::string eventType, int position);
};

#endif
