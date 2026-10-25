/*
    Developed for use in midi-interval-processor
    2019

    For detailed information on the MIDI file format, refer to: https://www.csie.ntu.edu.tw/~r92092/ref/midi/,
    which was used as a reference while writing this
*/

#include "noteGetter.h"
#include "musicalConcept.h"

#include <iomanip>
#include <sstream>
#include <list>
#include <stdexcept>
#include <string>

using namespace std;

/*
    Constructor for noteGetter, inFile must be a MIDI 1.0 file opened in binary mode
    Reads the whole file into stringHexMid, two lowercase hex characters per byte
*/
noteGetter::noteGetter(ifstream &inFile) {
    ostringstream hexStream;
    hexStream << hex << setfill('0');
    char byte;
    while (inFile.get(byte)) {
        hexStream << setw(2) << (int)(unsigned char)byte;
    }
    stringHexMid = hexStream.str();
}

/*
    Reads the variable length value starting at position in stringHexMid
    Adds its length in characters to length and its value to value (both should start at 0)
*/
void noteGetter::incrementLengthandValue(int position, int& length, int& value){
    const int msbBitMask = 0x80; //set on every byte of the value except the last
    int byte;
    do {
        byte = stoul(stringHexMid.substr(position + length, 2), nullptr, 16); //invalid_argument past the end of the file
        length += 2;
        value = (value << 7) + (byte & ~msbBitMask); //the MIDI format caps these at 4 bytes, so this can't overflow
    } while (byte & msbBitMask);
}

/*
    Returns the length of a variable length value
    Position is the start of the variable length value in the given stringHexMid
*/
int noteGetter::getLengthOfVariableLengthValue(int position){
    int value = 0;
    int length = 0;
    incrementLengthandValue(position, length, value);
    return length;
}

/*
    Returns the value given by a variable length value
    Position is the start of the variable length value in the given stringHexMid
*/
int noteGetter::getVariableLengthValue(int position) {
    int value = 0;
    int length = 0;
    incrementLengthandValue(position, length, value);
    return value;
}

/*
    Returns -1, as nothing follows an end of file message
*/
int noteGetter::indexAfterEndOfFileMessageAt() {
    lastStatusByte = ""; //like every message that isn't a MIDI event, this ends running status
    return -1;
}

/*
    Returns the position of the next message after a MThd header message
    Position is the start of the current message
*/
int noteGetter::indexAfterMThdAt(int position) {
    lastStatusByte = "";
    int length = stoul(stringHexMid.substr(position+8,8), nullptr, 16); //in bytes, and 1 byte = 2 chars
    return getNextNoteOnPosition(position + 16 + 2 * length); //skip "MThd", the length and the header data
}

/*
    Returns the position of the next message after a MTrk header message
    Position is the start of the current message
*/
int noteGetter::indexAfterMTrkAt(int position) {
    lastStatusByte = "";
    return indexAfterDeltaTimeEvent(position + 16); //skip "MTrk" and its 4 byte length; a delta time comes next
}

/*
    Returns the position of the next message after a sysex event F0 or F7 message
    Position is the start of the current message
*/
int noteGetter::indexAfterSysexEventF0orF7(int position) {
    lastStatusByte = "";
    int lengthVal = 2 * getVariableLengthValue(position+2); //the length is in bytes, and 1 byte = 2 chars
    lengthVal += getLengthOfVariableLengthValue(position+2) + 2; //+2 for the F0 or F7 itself
    return indexAfterDeltaTimeEvent(position+lengthVal);
}

/*
    Returns the position of the next message after a meta event message
    Position is the start of the current message
*/
int noteGetter::indexAfterMetaEvent(int position) {
    lastStatusByte = "";
    if (stringHexMid.substr(position+2,2) == "2f") { //end of track, always ff 2f 00
        if (nextPotentialChunkType(position + 6) == "4d54726b") {
            return getNextNoteOnPosition(position + 6); //another track follows
        }
        return -1; //no more tracks, so this is the end of the file
    }
    int lengthVal = 2 * getVariableLengthValue(position+4);
    lengthVal += getLengthOfVariableLengthValue(position+4) + 4; // +4 for the FF nn format
    return indexAfterDeltaTimeEvent(position+lengthVal);
}

/*
    Returns the position of the next note on message, starting from the MIDI event at position
    eventType is the event's first byte, which is a data byte if the event uses running status
*/
int noteGetter::indexAfterNextNoteOnMessage(string eventType, int position) {
    char statusType = eventType[0];
    bool isStatusByte = statusType == '8' or statusType == '9' or (statusType >= 'a' and statusType <= 'e');
    if (not isStatusByte) { //a data byte (00-7f), so this message uses running status
        if (lastStatusByte[0] == '9') {
            return position - 2; //a running status note on, read it as if the 9n were there
        }
        //skip the data bytes of the remembered status: cn and dn have one, others two
        char runningStatusType = lastStatusByte[0];
        if (runningStatusType == 'c' or runningStatusType == 'd') {
            return indexAfterDeltaTimeEvent(position + 2);
        } else {
            return indexAfterDeltaTimeEvent(position + 4);
        }
    } else { //a new status byte
        lastStatusByte = eventType;
        if (statusType == '9') {
            return position; //this is a note on message
        }
        if (statusType == 'c' or statusType == 'd') {
            return indexAfterDeltaTimeEvent(position + 4);
        } else {
            return indexAfterDeltaTimeEvent(position + 6);
        }
    }
}

/*
    Returns the 4 bytes at position, which may be a chunk type (MThd or MTrk)
*/
string noteGetter::nextPotentialChunkType(int position) {
    try {
        return stringHexMid.substr(position,8);
    } catch (const out_of_range&) {
        throw range_error("Got to the end of the file too early at position: "+to_string(position));
    }
}

/*
    Returns the position of the next message after a delta time MIDI message
    Position is the start of the current message
*/
int noteGetter::indexAfterDeltaTimeEvent (int position) {
    return getNextNoteOnPosition(position + getLengthOfVariableLengthValue(position));
}

/*
    Returns the position of the next note on message in the file, or -1 at the end of the file
    Position is the start of the current message
    This is a type of recursive descent parser
*/
int noteGetter::getNextNoteOnPosition(int position) {
    string potentialChunkType = nextPotentialChunkType(position);
    if (potentialChunkType == "ff2f0000") { //end of track followed by a 00 byte, taken as the end of the file
        return indexAfterEndOfFileMessageAt();
    } else if (potentialChunkType == "4d546864") { //MThd
        return indexAfterMThdAt(position);
    } else if (potentialChunkType == "4d54726b") { //MTrk
        return indexAfterMTrkAt(position);
    }
    string eventType = stringHexMid.substr(position,2);
    if (eventType == "f0" or eventType == "f7") {
        return indexAfterSysexEventF0orF7(position);
    } else if (eventType == "ff") {
        return indexAfterMetaEvent(position);
    }
    return indexAfterNextNoteOnMessage(eventType, position); //a MIDI event, with or without a status byte
}

/*
    Fills the given empty list with the notes of the note on messages in the file, and returns how many there are
*/
int noteGetter::populateListWithNotes(list<note> &notes) {
    if (stringHexMid.compare(0, 8, "4d546864") != 0) { //every MIDI file starts with an MThd chunk
        throw runtime_error("Not a MIDI file (no MThd header)");
    }
    int count = 0;
    int position = getNextNoteOnPosition(0);
    while (position != -1) {
        //position is a 9n note on status byte, then the note number and velocity
        int noteNumInt = stoul(stringHexMid.substr(position + 2, 2), nullptr, 16);
        int noteVelInt = stoul(stringHexMid.substr(position + 4, 2), nullptr, 16);
        //velocity 0 is a note off, and values above 127 aren't valid MIDI data
        if (noteVelInt >= 1 and noteVelInt <= 127 and noteNumInt <= 127) {
            notes.push_back(note(noteNumInt));
            count += 1;
        }
        position = indexAfterDeltaTimeEvent(position + 6); //a delta time follows the 3 byte message
    }
    return count;
}
