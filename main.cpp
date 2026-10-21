/*
    Developed for use in midi-interval-processor
    2019
*/

#include "midiToInterval.h"

#include <exception>
#include <iostream>

/*
    This file exists to run midi_to_interval from the command line
    Exits 0 on success, 1 if the file can't be read or parsed, 2 on bad usage
*/
int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " file.mid\n";
        return 2;
    }
    try {
        run(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return 1;
    }
    return 0;
}
