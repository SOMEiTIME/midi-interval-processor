#include "midiToInterval.h"
#include "noteGetter.h"
#include "musicalConcept.h"

#include <iostream>
#include <fstream>
#include <list>
#include <string>

using namespace std;

/*
    Boolean flags for printing information as the program is run
*/
bool PRINT_NOTE_COUNT = false;
bool PRINT_NOTE_NAMES = false;

/*
    Prints the note count of a given file, 
    if the flag PRINT_NOTE_COUNT is set to true
    fileName is the name of the file that is having it's notes counted
    notes is the list of notes found in the file
*/
void printNoteCount(string fileName, list<note> notes) {
    if(PRINT_NOTE_COUNT){
        std::cout << "For file: " << fileName << "\n The count of notes is " << std::dec << notes.size() << "\n"; 
    }
}

/*
    Prints the note names found in a given file
    if the flag PRINT_NOTE_NAMES is set to true
    fileName is the name of the file that is having it's notes counted
    notes is the list of notes found in the file
*/
void printNoteNames(string fileName, list<note> notes) {
    if (PRINT_NOTE_NAMES){
        std::cout << "Note names for file:\n";
        for (note x : notes){
            std::cout << x.toString() << " ";
        }
    }
}

/*
    Writes the list intervals to the outpu file stream outFile
*/
void writeIntervalsToFile(ofstream& outFile, list<interval> intervals) {
    for (interval x : intervals){ 
        outFile << x.toString() << " ";
    }
}

/*
    creates a list of intervals when given a list of in order notes
*/
list<interval> populateListWithIntervals(list<note> notes) {
    list<interval> intervals;
    note previous(0);
    bool firstItem = true;
    for (note x : notes) {
        //Convert the note #s into interval #s
        //Populate the list of intervals
        if (!firstItem) { //There isn't an interval if this is the first item
            intervals.push_back(interval(previous,x)); 
        }
        previous = x;
        firstItem = false;
    } 
    return intervals;
}

/*
    Takes a file name in string form and returns the count of notes in a file, 
    as well as potentially writing out a file called <FILENAME>_intervals.txt
    that lists the intervals in order as found in the MIDI file
*/
int run(string fileName) {

    ifstream inFile;
    inFile.open(fileName,ios::binary); //Open the input file (must be a MIDI file)
    int noteCount = 0;
    list<note> notes; //Will be the list of notes that are found in the file
    list<interval> intervals; //Will bethe list of intervals of the notes found in the file
    if (inFile.good()) {
        noteGetter noteGet(inFile);
        //Populate the list with all the notes found in the given input file of type .mid
        noteCount = noteGet.populateListWithNotes(notes);
        intervals = populateListWithIntervals(notes);
        inFile.close();
    } else {
        std::cerr << "Unable to open input file: \n" + fileName+" \n";
        exit(1);
    }
    ofstream outFile; 
    outFile.open(fileName + "_intervals.txt",ios::out); 
    if (outFile.good()) {
        writeIntervalsToFile(outFile, intervals); //Print out the list of intervals to the given output file
        outFile.close();
    } else {
        std::cerr << "Unable to open output file: \n" + fileName+" \n";
        exit(1);
    }
    printNoteCount(fileName, notes);
    printNoteNames(fileName, notes);    
    return noteCount;
}


