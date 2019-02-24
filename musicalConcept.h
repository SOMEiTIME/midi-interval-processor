#ifndef _musicalConcept_h
#define _musicalConcept_h

#include <map>
#include <string>

/*
    Abstract Class for the datatypes note and interval
*/
class musicalConcept{
    public:
        int toInt(); //musical concepts should easily be converted into integers
        virtual std::string toString() = 0; //used for printing values
    protected:
        int integerRepresentation; //every musicalConcept has a numeric identifier
};

/*
    Class to represent notes, is a musical concept
*/
class note: public musicalConcept{
    std::map<int,std::string> intNoteMap = {
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
    std::string intToNote(int x);
    public:
        note(int stepValue);
        std::string toString();
};

/*
    Class to represent intervals, is a musical concept
*/
class interval: public musicalConcept{
    public:
        interval(note previous, note current);
        std::string toString();
};

#endif