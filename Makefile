# makefile for midiToInterval code

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

.PHONY: all test clean

all: midiToInterval testMidiToInterval

test: testMidiToInterval
	./testMidiToInterval

noteGetter.o: noteGetter.h musicalConcept.h noteGetter.cpp
	$(CXX) $(CXXFLAGS) -c noteGetter.cpp

midiToInterval.o: midiToInterval.h noteGetter.h musicalConcept.h midiToInterval.cpp
	$(CXX) $(CXXFLAGS) -c midiToInterval.cpp

testMidiToInterval.o: midiToInterval.h musicalConcept.h testMidiToInterval.cpp
	$(CXX) $(CXXFLAGS) -c testMidiToInterval.cpp

main.o: midiToInterval.h musicalConcept.h main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

musicalConcept.o: musicalConcept.h musicalConcept.cpp
	$(CXX) $(CXXFLAGS) -c musicalConcept.cpp

midiToInterval: midiToInterval.o noteGetter.o main.o musicalConcept.o
	$(CXX) $(CXXFLAGS) midiToInterval.o main.o noteGetter.o musicalConcept.o -o midiToInterval

testMidiToInterval: midiToInterval.o testMidiToInterval.o noteGetter.o musicalConcept.o
	$(CXX) $(CXXFLAGS) -pthread midiToInterval.o testMidiToInterval.o noteGetter.o musicalConcept.o -lgtest -lpthread -o testMidiToInterval

clean:
	rm -f *.o midiToInterval testMidiToInterval
