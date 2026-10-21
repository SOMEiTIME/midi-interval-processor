# makefile for midiToInterval code

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

all: midiToInterval testMidiToInterval

test: testMidiToInterval
	./testMidiToInterval

noteGetter.o: noteGetter.h noteGetter.cpp
	$(CXX) $(CXXFLAGS) -c noteGetter.cpp

midiToInterval.o: midiToInterval.h midiToInterval.cpp
	$(CXX) $(CXXFLAGS) -c midiToInterval.cpp

testMidiToInterval.o: midiToInterval.h testMidiToInterval.cpp
	$(CXX) $(CXXFLAGS) -c testMidiToInterval.cpp

main.o: midiToInterval.h main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

musicalConcept.o: musicalConcept.h musicalConcept.cpp
	$(CXX) $(CXXFLAGS) -c musicalConcept.cpp

midiToInterval: midiToInterval.o noteGetter.o main.o musicalConcept.o
	$(CXX) $(CXXFLAGS) midiToInterval.o main.o noteGetter.o musicalConcept.o -o midiToInterval

testMidiToInterval: midiToInterval.o testMidiToInterval.o noteGetter.o musicalConcept.o
	$(CXX) $(CXXFLAGS) -pthread midiToInterval.o testMidiToInterval.o noteGetter.o musicalConcept.o -lgtest -lpthread -o testMidiToInterval

clean:
	rm -f *.o midiToInterval testMidiToInterval main
