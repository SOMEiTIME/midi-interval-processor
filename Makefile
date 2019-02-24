# makefile for midiToInterval code

all: midiToInterval testMidiToInterval

test: testMidiToInterval
	./testMidiToInterval

noteGetter.o: noteGetter.h noteGetter.cpp
	g++ -std=c++11 -Wall -c noteGetter.cpp

midiToInterval.o: midiToInterval.h midiToInterval.cpp
	g++ -std=c++11 -Wall -c midiToInterval.cpp

testMidiToInterval.o: midiToInterval.h testMidiToInterval.cpp 
	g++ -std=c++11 -Wall -c testMidiToInterval.cpp

main.o: midiToInterval.h main.cpp
	g++ -std=c++11 -Wall -c main.cpp

midiToInterval: midiToInterval.o noteGetter.o main.o
	g++ -std=c++11 -Wall midiToInterval.o main.o noteGetter.o -o midiToInterval 

testMidiToInterval: midiToInterval.o testMidiToInterval.o noteGetter.o
	g++ -pthread -std=c++11 -Wall midiToInterval.o testMidiToInterval.o noteGetter.o -lgtest_main -lgtest -lpthread -o testMidiToInterval

clean:
	rm -f *.o midiToInterval testMidiToInterval main
