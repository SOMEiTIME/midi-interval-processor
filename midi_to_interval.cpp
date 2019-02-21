
#include "midi_to_interval.h"
//#pragma



bool isVerbose = false;
bool isDisplayingTheFile = false; 
bool isShowingGetNextPosition = false;
bool showNoteCount = false;

bool catchPotentialNoteOnsInDeltaTimeEvents = false;
bool allowingUknownMidiMessages = true; //the midi specification says to allow for uknown messages

int stopPoint = -1; //set to -1 to not stop at a certain point in the midi file
int recursionLevel = 0; //tracks the times we have recursed
int recursionLimit = 999999999;
//int recursionLimit = 1000;
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
    int getLengthorValue(int position, string desired);
};

NoteGetter::NoteGetter(ifstream &inFile) {
    //int x;
    string str;
    inFile.seekg(0); //gotta reset the pointer to the start of the file!, When it was written, the file pointer got put to almost the end of the file
    ostringstream test;
    if (isDisplayingTheFile) {
        std::cout << "==\n";
    }
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
        if (isDisplayingTheFile) {
            std::cout << "stringBitForFormatting: "<< stringBitForFormatting << "---";  
        }   
        if (stringBitForFormatting.length() == 1) { //first issue, parts that are like 01 get shortened to 1
            tempBitForFormatting.clear();
            tempBitForFormatting.str("0" + stringBitForFormatting);
        } else if (stringBitForFormatting.find("ffffff") !=  string::npos){
            tempBitForFormatting.clear();
            tempBitForFormatting.str(stringBitForFormatting.substr(6,2));
        }
        if (isDisplayingTheFile) {
            std::cout << tempBitForFormatting.str() << "\n";  
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
int NoteGetter::getLengthorValue(int position, string desired){
    int length = 0;
    bool notTheEnd = true; 
    int value = 0;
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
        if (isShowingGetNextPosition) {
            std::cout << "  -deltaPart: " << deltaPart << "\n";
            std::cout << "  -deltaPartInt: " << deltaPartInt << "\n";
            std::cout << "  -value of deltaPartInt after getting rid of MSB: " << (int) valueOfDeltaPartInt << "\n";
        }
        value = value << 7; //I'm not concerned about overflow, because of the max value allowed of the variable length quntities by the midi format
        value += valueOfDeltaPartInt; 
    }
    if (isShowingGetNextPosition) {
            std::cout << "  length is: " << length << "\n"; //length of this VSQ as in #of characters, not bytes
            std::cout << "  value is: " << value << "\n";//value returns the length in bytes (as we all know, 2 byes = 1 char)
    }
    if (desired == "value") {
        return value;
    } else if (desired == "length"){
        return length;
    } else {
        std::cerr << "Didn't specify if value or length was desired\n";
        exit(1);
    }
}


//change later, its wrong now (get the get variable length value one working)
int NoteGetter::getLengthOfVariableLengthValue(int position){
    return getLengthorValue(position, "length");
}


/*
    returns the value given by a varaiable length value 
    position is the start of the variable length value in the given stringHexMid

*/
int NoteGetter::getVariableLengthValue(int position) {
    return getLengthorValue(position, "value");
}


/*
    
*/
int NoteGetter::getNextNoteOnPosition(int position, bool isADeltaTimeEvent) {//i'd like this to be a pointer, not an actual string (longer term optimisation)
    if (stopPoint != -1 and position >= stopPoint) {
        std::cerr << "\n\nStopped at position: " << position << "\n\n";
        assert(false); 
    } else if (recursionLevel >= recursionLimit) {//warning, this will stop your code
        std::cerr << "\n\nRecursed too many times, stopped at position: " << position << "\n\n";
        assert(false);
    }
    int oldPosition = position;
    string potentialChunkType;
    try {
        potentialChunkType = stringHexMid.substr(oldPosition,8);
    } catch(std::exception& e) {
        if (isShowingGetNextPosition) {
            std::cout << "Got to the end too early\n";
        }
        position = -1;
    }
    if (isShowingGetNextPosition) { 
        int lookBack = 32;
        if (position >= lookBack){
            cout <<"\n\n" <<stringHexMid.substr(oldPosition-lookBack,lookBack) << "." << stringHexMid.substr(oldPosition,32) << "\n";
        }
    }
    //looking at 4 byte value (represented by 8 chars)
    if (isADeltaTimeEvent){ //expecting a delta time event (so treat it like a delta time event)
        //figure out how long it the delta event is, and go forward that length
        //A variable-length quantity is a represented as a series of 7-bit values, from most-significant to least-significant. where the last byte of the series bit 7 (the most significant bit) set to 0, and the preceding bytes have bit 7 set to 1.
        if (isShowingGetNextPosition) {
                std::cout << "DELTA TIME EVENT: \n";     
        }   
        position += getLengthOfVariableLengthValue(position);
        recursionLevel++;
        if (isShowingGetNextPosition) {
            std::cout << "recursed to level: " << recursionLevel << "\n";
        }
        return getNextNoteOnPosition(position,false);//we know the next event will not be a deltaTimeEvent
    } else if (potentialChunkType == "ff2f0000") { //it's the end of the file! //this is a sysex event
        runningStatus = false;
        lastStatusByte = "";
        if (isShowingGetNextPosition) {
                std::cout << "EOF!\n";
        }
        position = -1;
        recursionLevel = 0;
        return position;
    } else if (potentialChunkType == "4d546864"){ //MThd = 4d546864
        runningStatus = false;
        lastStatusByte = "";
        if (isShowingGetNextPosition) {
            std::cout << "MThd header\n";
        } //if it's an MTrk header, skip forward by the length section
        string lengthString = stringHexMid.substr(oldPosition+8,8);
        int length = stoi(lengthString);
        //  "MTrk" takes 4 bytes and that takes 8 chars to represent, so 1 byte = 2 chars
        position += length * 4.75; //converts the length in bytes to length in chars( since we are navigating a string representation of this file)
        if (isShowingGetNextPosition) {
            std::cout << " header length: " << length <<"\n";
        }   
        recursionLevel++;
        if (isShowingGetNextPosition) {
            std::cout << "recursed to level: " << recursionLevel << "\n";
        }
        return getNextNoteOnPosition(position,false);
    } else if (potentialChunkType == "4d54726b") { // MTrk
        runningStatus = false;
        lastStatusByte = "";
        if (isShowingGetNextPosition) {
            std::cout << "MTrk header\n";
        } //go forward 8 more bytes (so 16 more characters) //this then points to a delta_time event
        //skips over the MTrk header and then the length part of the header (4 bytes after MTrk)
        // the section to go over has a variable length! in addition Mtrk headers are being lost
        recursionLevel++;
        if (isShowingGetNextPosition) {
            std::cout << "recursed to level: " << recursionLevel << "\n";
        }   
        return getNextNoteOnPosition(oldPosition + 16,true); //we know the next thing will be a delta time event
    } else {
        //its an event!
        if (isShowingGetNextPosition) {
            std::cout << "it's an EVENT of type: ";
        }
        string eventType = stringHexMid.substr(oldPosition,2); 
        if (eventType == "f0" or eventType == "f7") { //sysex event F0 or F7
            runningStatus = false; //don't exit running status for this type of event?
            lastStatusByte = "";
            if (isShowingGetNextPosition) {
                std::cout << "Sysex\n";
            }
            //find the length, skip forward  
            //length is in bytes
            //1 byte = 2 char
            int lengthVal = 2 * getVariableLengthValue(oldPosition+2);
            std::cerr << "getNextNotePosition F0 and F7 calling";
            lengthVal += getLengthOfVariableLengthValue(oldPosition+2) + 2; // for the F0 or F7 and  the length of the varable length value
            recursionLevel++;
            if (isShowingGetNextPosition) {
                std::cout << "recursed to level: " << recursionLevel << "\n";
            }
            return getNextNoteOnPosition(oldPosition+lengthVal, true);
        } else if (eventType == "ff") { //meta event
            runningStatus = false;
            lastStatusByte = "";
            if (isShowingGetNextPosition) {
                std::cout << "Meta\n";
            }
            string metaEventType = stringHexMid.substr(oldPosition+2,2);
            if (metaEventType == "2f") {
                    if (isShowingGetNextPosition) {
                        std::cout << "END OF TRACK\n";
                    }
                    recursionLevel++;
                    if (isShowingGetNextPosition) {
                            std::cout << "recursed to level: " << recursionLevel << "\n";
                    }
                    return getNextNoteOnPosition(oldPosition + 6, false); //expect a track header message , we know its length 6 because these are always FF 2f 00
                }           
            //find the length, skip forward
            int lengthVal = 2 * getVariableLengthValue(oldPosition+4); //is this offest right
            lengthVal += getLengthOfVariableLengthValue(oldPosition+4) + 4; // for the FF nn format and the length of the varable length value
            recursionLevel++;
            if (isShowingGetNextPosition) {
                std::cout << "recursed to level: " << recursionLevel << "\n";
            }
            return getNextNoteOnPosition(oldPosition+lengthVal, true);
        } else {    //this is a midi event of some type (with or without a midiEventByte)
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
                    if (isShowingGetNextPosition) {
                        std::cout << "We think this is a runningStatus message for a note on event\n";
                    }
                    recursionLevel = 0;
                    return (position - 2); //interprets this message like a 9n was there 
                }
                //else, handle like any other midi message (so skip over it by 4 or 6 characters)
                //cn and dn daya bytes are 2 characters long
                //others are 4 characters long
                recursionLevel++;
                if (isShowingGetNextPosition) {
                    std::cout << "recursed to level: " << recursionLevel << "\n";
                }
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
                    if (isShowingGetNextPosition) {
                        std::cout << "-------------------------MIDI NOTE ON MESSAGE FOUND\n";
                    }
                    recursionLevel = 0;
                    return position; //this is the noteOnMessage FINALLY
                }
                recursionLevel++;
                if (isShowingGetNextPosition) {
                    std::cout << "recursed to level: " << recursionLevel << "\n";
                }
                if (statusType == "c" or statusType == "d") {
                    return getNextNoteOnPosition(position + 4,true); //including 2 more characters (or 1 more byte for the status byte)
                } else {
                    return getNextNoteOnPosition(position + 6,true); 
                }
            }
        }
    }
}



/*
    Given an open .mid filestream, and an empty list of numerical representation of notes, it will fill that list of notes with the notes on messages found in the .mid file
 */
int NoteGetter::populateListWithNotes(list<int> &notes) {

    //the tempFile, and therefore stringHexMid is a string representation of the .mid file (verified) --other than a harmless 00 stuck on the end
    if(isVerbose){
        std::cout << "\n==\n";
        std::cout << stringHexMid;
    }
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
        if (isVerbose){
            std::cout << strLine << "\n";
            std::cout << " noteNumHex: " << noteNumHex << "\n";
            std::cout << " velocityNumHex: " << velocityNumHex << "\n";
        }
            //exlude notes with velocity 0, as those are "note off" messages commonly used in lots of .mid files
        //int channelNumInt = std::stoul(channelMessage, nullptr, 16); 
        int noteNumInt = std::stoul(noteNumHex, nullptr, 16); //converts the string note on value in hex to an int
        int noteVelInt = std::stoul(velocityNumHex, nullptr, 16);
        //if (noteNumInt > 21 and noteVelInt >= 5 and noteVelInt <= 127) { //exlude the false notes that are lower than the bottom of a piano
        if (noteVelInt >= 1 and noteVelInt <=127 and noteNumInt >= 0 and noteNumInt <= 127) {
        //exlude midi messages that don't make sense, at least
            if (isVerbose) {
                std::cout << "     note Name :" << int_to_note(noteNumInt) << "\n";
            }
            notes.push_back(noteNumInt);
            count += 1;
        } 
         //we just processed a note on message, and so the next expected event is a delta time event (after 6 chars (or 3 bytes) of midi note on message)
        if (runningStatus) {
            //the +6 accounts for a status bit, if this is a running status note, its +4
            recursionLevel++;
            if (isShowingGetNextPosition) {
                std::cout << "recursed to level: "<< recursionLevel << "\n";
            }
            position =  getNextNoteOnPosition(position+6,true); //we arent expecting a deltaTime when the status is running?
        } else {                                                                
            runningStatus = true;
            recursionLevel++;
            if (isShowingGetNextPosition) {
                std::cout << "recursed to level: "<< recursionLevel << "\n";
            }
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
    //std::cout << "hello\n";
    ifstream inFile;
    //open the input file (has to be a MIDI file)
    inFile.open(fileName,ios::binary);
    if (!inFile) {
        std::cerr << "--Unable to open input file: \n" + fileName+" \n";
        exit(1); // terminate with error
    }
    list<int> notes; //the list of notes that are found in the file
    list<int> intervals; //the list of intervals of the notes found in the file
    NoteGetter noteGet(inFile);
    //populate the list with all the notes found in the given input file of type .mid
    int noteCount = noteGet.populateListWithNotes(notes);
    int previous = -1;
    if (isVerbose) {
        std::cout << "\n   Notes: ";
    }
    for (int x : notes) {
        //convert the note #s into note names, and print
        if (isVerbose) {
            std::cout << int_to_note(x) << " ";
        }
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
        std::cerr << "Unable to open output file: \n" + fileName+" \n";
        exit(1); // terminate with error
    }
    if (isVerbose) {
        std::cout << "\nIntervals: ";
    }
    //print out the list of intervals to the given output file
    for (int x : intervals){ 
        outFile << std::dec << (x+288) % 12 << " "; //adding 288 keeps x positive
        if (isVerbose) {
            std::cout << std::dec << (x+288) % 12 << " ";
        }
    }
    if (isVerbose) {
        std::cout << "\n";
    }
    outFile.close();
    return noteCount;
}


