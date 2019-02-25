/*
    Developed for use in midi-interval-processer 
    Benjamin Hasker
    2019
*/

#ifndef _midiToInterval_h
#define _midiToInterval_h

#include "musicalConcept.h"

#include <string>
#include <list>
#include <fstream>
extern bool PRINT_NOTE_COUNT;
extern bool PRINT_NOTE_NAMES;

void printNoteCount(std::string fileName, std::list<note> notes);
void printNoteNames(std::string fileName, std::list<note> notes);
void writeIntervalsToFile(std::ofstream& outFile, std::list<interval> intervals);
std::list<interval> populateListWithIntervals(std::list<note> notes);
int run(std::string fileName);

#endif