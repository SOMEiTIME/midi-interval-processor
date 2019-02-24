#ifndef _noteGetter_h
#define _noteGetter_h

#include "musicalConcept.h"

#include <fstream>
#include <string>
#include <list>

/*
    Class noteGetter
    Instantiated for each file analyzed
    The main function is populateListWithNotes, which finds all the notes in a midi file and adds them to a given list
*/
class noteGetter {
    int runningStatus;
    std::string lastStatusByte;
    std::string stringHexMid; //string representation of the hex representation of the MIDI file, read in when noteGetter is instantiated
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
    int indexAfterEndOfFileMessageAt(int position);
    int indexAfterMThdAt(int position);
    int indexAfterMTrkAt(int position);
    int indexAfterSysexEventF0orF7(int position);
    int indexAfterMetaEvent(int oldPosition);
    int indexAfterNextNoteOnMessage(std::string eventType, int position);
};

#endif
