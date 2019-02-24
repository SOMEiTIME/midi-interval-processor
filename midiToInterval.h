#ifndef _midiToInterval_h
#define _midiToInterval_h
#include <string>

extern bool PRINT_NOTE_COUNT;
extern bool PRINT_NOTE_NAMES;

std::string intToNote(int x); 

int run(std::string fileName);

#endif