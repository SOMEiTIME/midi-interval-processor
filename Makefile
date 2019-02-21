# makefile for cube code

all: midi_to_interval test_midi_to_interval

test: test_midi_to_interval
	./test_midi_to_interval

midi_to_interval.o: midi_to_interval.h midi_to_interval.cpp
	g++ -std=c++11 -Wall -c midi_to_interval.cpp

test_midi_to_interval.o: midi_to_interval.h test_midi_to_interval.cpp
	g++ -std=c++11 -Wall -c test_midi_to_interval.cpp

main.o: midi_to_interval.h main.cpp
	g++ -std=c++11 -Wall -c main.cpp

midi_to_interval: midi_to_interval.o main.o
	g++ -std=c++11 -Wall midi_to_interval.o main.o -o midi_to_interval

test_midi_to_interval: midi_to_interval.o test_midi_to_interval.o
	g++ -pthread -std=c++11 -Wall midi_to_interval.o test_midi_to_interval.o -lgtest_main -lgtest -lpthread -o test_midi_to_interval

clean:
	rm -f *.o midi_to_interval test_midi_to_interval main
