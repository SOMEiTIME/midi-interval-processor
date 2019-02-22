#include "midi_to_interval.h"
#include "noteGetter.h"

bool showNoteCount = false;
bool isPrintingNoteNames = false;

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
};

/*  takes an int, and returns a string representation of the note name, 
    exluding flats and complex spellings of simple notes (like E# for F)
*/
string int_to_note(int x) {
    int remainder = x % 12;
    return intNoteMap[remainder];
}


/*
    Run takes a file name in string form and returns the count of notes in a file, 
    as well as potentially writing out a file called <FILENAME>_intervals.txt
    that lists the intervals in order as found in the MIDI file
*/
int run(string fileName) {
    ifstream inFile;
    //open the input file (has to be a MIDI file)
    inFile.open(fileName,ios::binary);
    int noteCount = 0;
    list<int> notes; //the list of notes that are found in the file
    list<int> intervals; //the list of intervals of the notes found in the file

    if (inFile.good()) {
        noteGetter noteGet(inFile);
        //populate the list with all the notes found in the given input file of type .mid
        noteCount = noteGet.populateListWithNotes(notes);
        int previous = -1;
        if (isPrintingNoteNames){
            std::cout << "Note Names:\n";
        }
        for (int x : notes) {
            if (isPrintingNoteNames) {
                std::cout << int_to_note(x) << " ";
            }
            //convert the note #s into note names, and print
            //convert the note #s into interval #s
            //populate the list of intervals
            if (previous != -1) { //there isn't an interval if this is the first item
                intervals.push_back(abs(x-previous)); //putting in abs means B to C and C to B are the same interval, which they are
            }
            previous = x;   
        } 
        inFile.close();
    } else {
        std::cerr << "Unable to open input file: \n" + fileName+" \n";
        exit(1);
    }
    if (showNoteCount) {
        std::cout << "For File: " << fileName << "\n The count of notes is " << std::dec << notes.size() << "\n"; 
    }

    ofstream outFile; 
    outFile.open(fileName + "_intervals.txt",ios::out); 
    
    if (outFile.good()) {
        for (int x : intervals){ 
            outFile << std::dec << (x+288) % 12 << " "; //adding 288 keeps x positive
        }
    outFile.close();

    } else {
        std::cerr << "Unable to open output file: \n" + fileName+" \n";
        exit(1);
    }
    //print out the list of intervals to the given output file
    return noteCount;
}


