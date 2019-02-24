#include "midiToInterval.h"
#include "noteGetter.h"
#include "musicalConcept.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <list>
#include <map>
#include <cstring>
#include <assert.h>
#include <string>

using namespace std;

bool PRINT_NOTE_COUNT = false;
bool PRINT_NOTE_NAMES = false;



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
    list<note> notes; //the list of notes that are found in the file
    list<interval> intervals; //the list of intervals of the notes found in the file
    if (inFile.good()) {
        noteGetter noteGet(inFile);
        //populate the list with all the notes found in the given input file of type .mid
        noteCount = noteGet.populateListWithNotes(notes);
        note previous(0);
        bool firstItem = true;
        
        if (PRINT_NOTE_NAMES){
            std::cout << "Note Names:\n";
        }
        for (note x : notes) {
            if (PRINT_NOTE_NAMES) {
                std::cout << x.toString() << " ";
            }
            //convert the note #s into note names, and print
            //convert the note #s into interval #s
            //populate the list of intervals
            if (!firstItem) { //there isn't an interval if this is the first item
                intervals.push_back(interval(previous,x)); 
            }
            previous = x;
            firstItem = false;
        } 
        inFile.close();
    } else {
        std::cerr << "Unable to open input file: \n" + fileName+" \n";
        exit(1);
    }
    if (PRINT_NOTE_COUNT) {
        std::cout << "For File: " << fileName << "\n The count of notes is " << std::dec << notes.size() << "\n"; 
    }
    ofstream outFile; 
    outFile.open(fileName + "_intervals.txt",ios::out); 
    if (outFile.good()) {
        for (interval x : intervals){ 
            outFile << x.toString() << " "; //adding 288 keeps x positive
        }
        outFile.close();
    } else {
        std::cerr << "Unable to open output file: \n" + fileName+" \n";
        exit(1);
    }
    //print out the list of intervals to the given output file
    return noteCount;
}


