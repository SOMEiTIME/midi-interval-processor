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
int run(std::string fileName);

#endif