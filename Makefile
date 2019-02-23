# makefile for midi_to_interval code

all: midi_to_interval test_midi_to_interval

test: test_midi_to_interval
	./test_midi_to_interval

noteGetter.o: noteGetter.h noteGetter.cpp
	g++ -std=c++11 -Wall -c noteGetter.cpp

midi_to_interval.o: midi_to_interval.h midi_to_interval.cpp
	g++ -std=c++11 -Wall -c midi_to_interval.cpp

test_midi_to_interval.o: midi_to_interval.h test_midi_to_interval.cpp 
	g++ -std=c++11 -Wall -c test_midi_to_interval.cpp

main.o: midi_to_interval.h main.cpp
	g++ -std=c++11 -Wall -c main.cpp

midi_to_interval: noteGetter.o midi_to_interval.o main.o
	g++ -std=c++11 -Wall midi_to_interval.o main.o noteGetter.o -o midi_to_interval 

test_midi_to_interval: midi_to_interval.o test_midi_to_interval.o noteGetter.o
	g++ -pthread -std=c++11 -Wall midi_to_interval.o test_midi_to_interval.o noteGetter.o -lgtest_main -lgtest -lpthread -o test_midi_to_interval

clean:
	rm -f *.o midi_to_interval test_midi_to_interval main
