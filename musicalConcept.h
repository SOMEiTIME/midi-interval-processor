/*
    Developed for use in midi-interval-processor
    2019
*/

#ifndef _musicalConcept_h
#define _musicalConcept_h

#include <string>

/*
    Abstract Class for the datatypes note and interval
*/
class musicalConcept{
    public:
        int toInt(); //musical concepts should easily be converted into integers
        virtual std::string toString() = 0; //used for printing values
        virtual ~musicalConcept() = default; //so a note or interval is safe to delete through a musicalConcept pointer
    protected:
        int integerRepresentation; //every musicalConcept has a numeric identifier
};

/*
    Class to represent notes, is a musical concept
*/
class note: public musicalConcept{
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