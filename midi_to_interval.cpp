#include "midi_to_interval.h"
//#pragma

bool showNoteCount = false;

bool catchPotentialNoteOnsInDeltaTimeEvents = false;
bool allowingUknownMidiMessages = true; //the midi specification says to allow for uknown messages

map<int,string> intNoteMap = {
    {0,"C"},
    {1,"C#"},
    {2,"D"},
    {3,"D#"},
    {4,"E"},
    {5,"F"},
    {6,"F#"},
    {7,"G"},
    {8,"G#"},
    {9,"A"},
    {10,"A#"},
    {11,"B"},
    //and 12 wraps around to C again (so use mod)
};  //also could have used enum


//  use 
//  g++ -std=c++11 midi_to_interval.cpp
/*  takes an int, and returns a string representation of the note name, 
    exluding flats and complex spellings of simple notes (like E# for F)
*/
string int_to_note(int x) {
    int remainder = x % 12;
    return intNoteMap[remainder];
}

class NoteGetter {
    int runningStatus = false;
    string lastStatusByte = "";
    string stringHexMid = ""; //string representation of the hex representation of the MIDI file, read in when noteGetter is instantiated
    //int position = 0; //position into stringHexMid
    public:
        int populateListWithNotes(list<int> &notes);
        NoteGetter(ifstream &inFile);
    int getNextNoteOnPosition(int position, bool isADeltaTimeEvent);
    int getVariableLengthValue(int position);
    int getLengthOfVariableLengthValue(int position);
    void incrementLengthandValue(int& position, int& length, int& value);

    string nextPotentialChunkType(int position);
    int indexOfEndOfFileMessageAt(int position);
    int indexOfMThdAt(int position);
    int indexOfMTrkAt(int position);
    int indexOfSysexEventF0orF7(int position);
    int indexOfMetaEvent(int oldPosition);
    int indexOfNextNoteOnMessage(string eventType, int position);
    
};


NoteGetter::NoteGetter(ifstream &inFile) {
    //int x;
    string str;
    inFile.seekg(0); //gotta reset the pointer to the start of the file!, When it was written, the file pointer got put to almost the end of the file
    ostringstream test;
    stringstream buffer;
    stringstream tempBitForFormatting;
    char smallPart;
    while (!inFile.eof()) {
        //read returns unformatted data in the size of one character so M, then t, then H, then D
        inFile.read(&smallPart,1); 
        tempBitForFormatting.str("");  //don't delete, this clears the tempBitForFormatting
        tempBitForFormatting << std::hex << (int) smallPart; //casts each character to an int (I don't like casting like this)
                                            //converts that int into a sting
                //then converts that string to hex
        //formatting stuff
        string stringBitForFormatting = tempBitForFormatting.str();

        if (stringBitForFormatting.length() == 1) { //first issue, parts that are like 01 get shortened to 1
            tempBitForFormatting.clear();
            tempBitForFormatting.str("0" + stringBitForFormatting);
        } else if (stringBitForFormatting.find("ffffff") !=  string::npos){
            tempBitForFormatting.clear();
            tempBitForFormatting.str(stringBitForFormatting.substr(6,2));
        }

        buffer << tempBitForFormatting.str();
        //tempFsile << tempBitForFormatting.str(); //puts the string into the temp file (mainly for debug purposes)
        //for large files, for some reason this doesn't all go in, but rest assured, the stringHexMid makes it work
        tempBitForFormatting.str(""); //don't delete, this clears the tempBitForFormatting
    }
    stringHexMid = buffer.str(); //I'm confident is this is a string representation of the hex interpretation of the binary .mid file
    inFile.seekg(0); //have to reset the pointer to the start of the file!, When it was written, the file pointer got put to almost the end of the file
}


/*
    Helper for VariableLengthValues, used in the getLengthOfVariableLengthValue and getVariableLengthValue
    desired determines what is returned. If desired is "value", 
    the value of the varaible length value is returned, if desired is "length" the length is returned
    otherwise there is an error
*/
void NoteGetter::incrementLengthandValue(int& position, int& length, int& value){
    bool notTheEnd = true; 
    stringstream valueString;
    while (notTheEnd) {
        string deltaPart = "";
        deltaPart = stringHexMid.substr(position + length,2);//so we load up one byte a time (2 char's worth)
        //convert this string representation in hex to int variable, then convert that int into binary (oh wait, ints are stored in binary)
        int deltaPartInt = std::stoul(deltaPart, nullptr, 16); //converts the string note on value in hex to an int
        const unsigned char msbBitMask = 128; //0000 0001
        //1111 0101 would be not the end of the delta time event
    //   &1000 0000
        //result: 1000 0000  so a nonzero result means that we aren't at the end
        //0111 1111 would be the end of the delta time event
        unsigned char result = deltaPartInt & msbBitMask;
        if (result == 0) { //this means we are at the end of the delta time event woohoo
            notTheEnd = false;
        }
        length +=2;
        unsigned char valueOfDeltaPartInt = deltaPartInt & ~msbBitMask;
        value = value << 7; //I'm not concerned about overflow, because of the max value allowed of the variable length quntities by the midi format
        value += valueOfDeltaPartInt; 
    }
    return;
}


int NoteGetter::getLengthOfVariableLengthValue(int position){
    int value = 0;
    int length = 0;
    incrementLengthandValue(position, length, value);
    return length;
}


/*
    returns the value given by a varaiable length value 
    position is the start of the variable length value in the given stringHexMid

*/
int NoteGetter::getVariableLengthValue(int position) {
    int value = 0;
    int length = 0;
    incrementLengthandValue(position, length, value);
    return value;
}

int NoteGetter::indexOfEndOfFileMessageAt(int position) {
    runningStatus = false;
    lastStatusByte = "";
    position = -1;
    return position;
}

int NoteGetter::indexOfMThdAt(int position) {
    runningStatus = false;
    lastStatusByte = "";
    //if it's an MTrk header, skip forward by the length section
    string lengthString = stringHexMid.substr(position+8,8);
    int length = stoi(lengthString);
    //  "MTrk" takes 4 bytes and that takes 8 chars to represent, so 1 byte = 2 chars
    position += length * 4.75; //converts the length in bytes to length in chars( since we are navigating a string representation of this file)
    return getNextNoteOnPosition(position,false);
}

int NoteGetter::indexOfMTrkAt(int position) {
    runningStatus = false;
    lastStatusByte = "";
    //MTrk header
     //go forward 8 more bytes (so 16 more characters) //this then points to a delta_time event
    //skips over the MTrk header and then the length part of the header (4 bytes after MTrk)
    // the section to go over has a variable length! in addition Mtrk headers are being lost  
    return getNextNoteOnPosition(position + 16,true); //we know the next thing will be a delta time event
}

int NoteGetter::indexOfSysexEventF0orF7(int position) {
    runningStatus = false; //don't exit running status for this type of event?
    lastStatusByte = "";
    //find the length, skip forward  
    //length is in bytes
    //1 byte = 2 char
    int lengthVal = 2 * getVariableLengthValue(position+2);
    //"getNextNotePosition F0 and F7 calling";
    lengthVal += getLengthOfVariableLengthValue(position+2) + 2; // for the F0 or F7 and  the length of the varable length value
    return getNextNoteOnPosition(position+lengthVal, true);
}

int NoteGetter::indexOfMetaEvent(int position) {
    runningStatus = false;
    lastStatusByte = "";            
    string metaEventType = stringHexMid.substr(position+2,2);
    if (metaEventType == "2f") {
            //std::cout << "END OF TRACK\n";            
            return getNextNoteOnPosition(position + 6, false); //expect a track header message , we know its length 6 because these are always FF 2f 00
        }           
    //find the length, skip forward
    int lengthVal = 2 * getVariableLengthValue(position+4); //is this offest right
    lengthVal += getLengthOfVariableLengthValue(position+4) + 4; // for the FF nn format and the length of the varable length value
    return getNextNoteOnPosition(position+lengthVal, true);
}

int NoteGetter::indexOfNextNoteOnMessage(string eventType, int position) {
    string potentialMidiEventByte = eventType.substr(0,2);
    string statusType = potentialMidiEventByte.substr(0,1);
    bool isRunningStatusMessage = not (statusType == "8" 
        or statusType == "9" 
        or statusType == "a" 
        or statusType == "b" 
        or statusType == "c"
        or statusType == "d"
        or statusType == "e"); //data bytes will never start with an 8-e
    if (isRunningStatusMessage) {       //it is the result of a running status style message (no midi event status byte) 
        if (lastStatusByte.substr(0,1) == "9") { //we are in the "note-on" status
                //std::cout << "We think this is a runningStatus message for a note on event\n"; 
            return (position - 2); //interprets this message like a 9n was there 
        }
        //else, handle like any other midi message (so skip over it by 4 or 6 characters)
        //cn and dn daya bytes are 2 characters long
        //others are 4 characters long
        if (statusType == "c" or statusType == "d") {
            return getNextNoteOnPosition(position + 2,true); 
        } else {
            return getNextNoteOnPosition(position + 4,true); 
        }
    } else {        //or it is a new midi status-byte (of type 9?)
        //should make the new running status
        string midiEventByte = potentialMidiEventByte; //potentialMidiEventByte is now known to be the midi event byte
        lastStatusByte = midiEventByte;
        runningStatus = true;
        if (midiEventByte.substr(0,1) == "9"){
                //std::cout << "-------------------------MIDI NOTE ON MESSAGE FOUND\n";
            
            return position; //this is the noteOnMessage FINALLY
        }

        if (statusType == "c" or statusType == "d") {
            return getNextNoteOnPosition(position + 4,true); //including 2 more characters (or 1 more byte for the status byte)
        } else {
            return getNextNoteOnPosition(position + 6,true); 
        }
    }
}

string NoteGetter::nextPotentialChunkType(int position) {
    string potentialChunkType = "";
    try {
        potentialChunkType = stringHexMid.substr(position,8);
    } catch(std::exception& e) {
        throw "Got to the end too early\n";
    }
    return potentialChunkType;
}

/*
      
*/
int NoteGetter::getNextNoteOnPosition(int position, bool isADeltaTimeEvent) {//i'd like this to be a pointer, not an actual string (longer term optimisation)
    string potentialChunkType = nextPotentialChunkType(position);
    //looking at 4 byte value (represented by 8 chars)
    if (isADeltaTimeEvent){ //expecting a delta time event (so treat it like a delta time event)
        //figure out how long it the delta event is, and go forward that length
        //A variable-length quantity is a represented as a series of 7-bit values, 
        //from most-significant to least-significant. where the last byte of the series bit 7 
        //(the most significant bit) set to 0, and the preceding bytes have bit 7 set to 1.
        position += getLengthOfVariableLengthValue(position);
        return getNextNoteOnPosition(position,false);//we know the next event will not be a deltaTimeEvent
    } else if (potentialChunkType == "ff2f0000") { //it's the end of the file! //this is a sysex event
        return indexOfEndOfFileMessageAt(position);
    } else if (potentialChunkType == "4d546864"){ //MThd = 4d546864
        return indexOfMThdAt(position);
    } else if (potentialChunkType == "4d54726b") { // MTrk
        return indexOfMTrkAt(position); //we know the next thing will be a delta time event
    } else {
        //its an event!
        //it's an EVENT of type: 
        string eventType = stringHexMid.substr(position,2); 
        if (eventType == "f0" or eventType == "f7") { //sysex event F0 or F7
            return indexOfSysexEventF0orF7(position);
        } else if (eventType == "ff") { //meta event
            return indexOfMetaEvent(position);
        } else {    //this is a midi event of some type (with or without a midiEventByte)
            return indexOfNextNoteOnMessage(eventType, position);
        }
    }
}


/*
    Given an open .mid filestream, and an empty list of numerical representation of notes, it will fill that list of notes with the notes on messages found in the .mid file
 */
int NoteGetter::populateListWithNotes(list<int> &notes) {
    //the tempFile, and therefore stringHexMid is a string representation of the .mid file (verified) --other than a harmless 00 stuck on the end

    int count = 0;
    int position = 0;
    position = getNextNoteOnPosition(position,false);  
    string line, noteOnMessage, noteNumHex, velocityNumHex, channelMessage;
    while (position != -1) {
        string strLine = stringHexMid.substr(position,6);
        channelMessage = strLine.substr(1,1);
        noteOnMessage = strLine.substr(2,4);  
        noteNumHex = noteOnMessage.substr(0,2);
        velocityNumHex = noteOnMessage.substr(2,2);

            //exlude notes with velocity 0, as those are "note off" messages commonly used in lots of .mid files
        //int channelNumInt = std::stoul(channelMessage, nullptr, 16); 
        int noteNumInt = std::stoul(noteNumHex, nullptr, 16); //converts the string note on value in hex to an int
        int noteVelInt = std::stoul(velocityNumHex, nullptr, 16);
        //if (noteNumInt > 21 and noteVelInt >= 5 and noteVelInt <= 127) { //exlude the false notes that are lower than the bottom of a piano
        if (noteVelInt >= 1 and noteVelInt <=127 and noteNumInt >= 0 and noteNumInt <= 127) {
        //exlude midi messages that don't make sense, at least

            notes.push_back(noteNumInt);
            count += 1;
        } 
         //we just processed a note on message, and so the next expected event is a delta time event (after 6 chars (or 3 bytes) of midi note on message)
        if (runningStatus) {
            //the +6 accounts for a status bit, if this is a running status note, its +4

            position =  getNextNoteOnPosition(position+6,true); //we arent expecting a deltaTime when the status is running?
        } else {                                                                
            runningStatus = true;
            position =  getNextNoteOnPosition(position+6,true);
        }
    } 
    assert(count >= 0);
    if (showNoteCount) {
        std::cout << "     The count of notes is " << std::dec << count << "\n"; //-4 because of the stupid hack
    }
    return count;
}


/*
//main needs to be disconnected from the midi_to_interval functionality
int main(int argc, char *argv[] ) {
    return run(argc,argv[1]);
}
*/
int run(string fileName) {
    ifstream inFile;
    //open the input file (has to be a MIDI file)
    inFile.open(fileName,ios::binary);
    if (!inFile) {
        throw "Unable to open input file: \n" + fileName+" \n";
    }
    list<int> notes; //the list of notes that are found in the file
    list<int> intervals; //the list of intervals of the notes found in the file
    NoteGetter noteGet(inFile);
    //populate the list with all the notes found in the given input file of type .mid
    int noteCount = noteGet.populateListWithNotes(notes);
    int previous = -1;
    for (int x : notes) {
        //convert the note #s into note names, and print
        //convert the note #s into interval #s
        //populate the list of intervals
        if (previous != -1) { //there isn't an interval if this is the first item
            intervals.push_back(abs(x-previous)); //putting in abs means B to C and C to B are the same interval, which they are
        }
        previous = x;   
    } 
    inFile.close();
    ofstream outFile; 
    outFile.open(fileName + "_intervals.txt",ios::out); 
    if (!outFile) {
        throw "Unable to open output file: \n" + fileName+" \n";
    }
    //print out the list of intervals to the given output file
    for (int x : intervals){ 
        outFile << std::dec << (x+288) % 12 << " "; //adding 288 keeps x positive
    }

    outFile.close();
    return noteCount;
}


