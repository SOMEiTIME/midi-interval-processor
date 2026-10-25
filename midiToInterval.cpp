/*
    Developed for use in midi-interval-processor
    2019
*/

#include "midiToInterval.h"
#include "noteGetter.h"
#include "musicalConcept.h"

#include <iostream>
#include <fstream>
#include <list>
#include <stdexcept>
#include <string>

using namespace std;

/*
    Debug flags: print each file's note count or note names as it is run
*/
bool PRINT_NOTE_COUNT = false;
bool PRINT_NOTE_NAMES = false;

/*
    Prints the note count of a given file, if PRINT_NOTE_COUNT is set
*/
void printNoteCount(string fileName, list<note> notes) {
    if (PRINT_NOTE_COUNT) {
        cout << "For file: " << fileName << "\n The count of notes is " << notes.size() << "\n";
    }
}

/*
    Prints the names of the notes in a given file, if PRINT_NOTE_NAMES is set
*/
void printNoteNames(string fileName, list<note> notes) {
    if (PRINT_NOTE_NAMES) {
        cout << "Note names for file: " << fileName << "\n";
        for (note x : notes) {
            cout << x.toString() << " ";
        }
    }
}

/*
    Writes the intervals to outFile, each followed by a space
*/
void writeIntervalsToFile(ofstream& outFile, list<interval> intervals) {
    for (interval x : intervals) {
        outFile << x.toString() << " ";
    }
}

/*
    Returns the intervals between each pair of consecutive notes
*/
list<interval> populateListWithIntervals(list<note> notes) {
    list<interval> intervals;
    for (auto it = notes.begin(); it != notes.end() and next(it) != notes.end(); ++it) {
        intervals.push_back(interval(*it, *next(it)));
    }
    return intervals;
}

/*
    Writes the intervals between the notes of the MIDI file fileName, in order, to <FILENAME>_intervals.txt
    Returns the note count
    Throws std::runtime_error if a file can't be opened or the input isn't a readable MIDI file
*/
int run(string fileName) {
    ifstream inFile(fileName, ios::binary);
    if (!inFile.good()) {
        throw runtime_error("Unable to open input file: " + fileName);
    }
    list<note> notes;
    int noteCount;
    noteGetter noteGet(inFile);
    //Reading past the end of the data shows up as out_of_range (substr) or invalid_argument (stoul on an empty string)
    try {
        noteCount = noteGet.populateListWithNotes(notes);
    } catch (const out_of_range&) {
        throw runtime_error("Truncated or malformed MIDI file: " + fileName);
    } catch (const invalid_argument&) {
        throw runtime_error("Truncated or malformed MIDI file: " + fileName);
    }
    ofstream outFile(fileName + "_intervals.txt");
    if (!outFile.good()) {
        throw runtime_error("Unable to open output file: " + fileName + "_intervals.txt");
    }
    writeIntervalsToFile(outFile, populateListWithIntervals(notes));
    printNoteCount(fileName, notes);
    printNoteNames(fileName, notes);
    return noteCount;
}
