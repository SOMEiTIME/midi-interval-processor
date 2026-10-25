/*
    Developed for use in midi-interval-processor
    2019
*/

#include "midiToInterval.h"
#include "musicalConcept.h"
#include <gtest/gtest.h>

#include <cstdint>
#include <fstream>
#include <initializer_list>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

/*
    Tests for use with google test
*/
string folder = "testData/";
string folderBeethoven = folder + "BeethovenSymphony3-mids/";
string folderMozart = folder + "magicflute-17-aria-mids/";

typedef vector<uint8_t> bytes;

/*
    Appends value to out as a 4 byte big-endian number, the way MIDI stores chunk lengths
*/
void appendLength(bytes& out, uint32_t value) {
    for (int shift = 24; shift >= 0; shift -= 8) {
        out.push_back((value >> shift) & 0xff);
    }
}

/*
    Builds a MIDI file from the event bytes of each track
    Each track should end with an end of track event (00 ff 2f 00)
*/
bytes midiFile(initializer_list<bytes> tracks) {
    bytes file = {'M', 'T', 'h', 'd'};
    appendLength(file, 6);
    file.insert(file.end(), {0x00, 0x01, 0x00, (uint8_t)tracks.size(), 0x00, 0x60}); //format 1, 96 ticks per beat
    for (const bytes& track : tracks) {
        file.insert(file.end(), {'M', 'T', 'r', 'k'});
        appendLength(file, track.size());
        file.insert(file.end(), track.begin(), track.end());
    }
    return file;
}

/*
    Writes contents to testData/built_<name> (gitignored) and returns its path
*/
string writeTestFile(const string& name, const bytes& contents) {
    string path = folder + "built_" + name;
    ofstream out(path, ios::binary);
    out.write((const char*)contents.data(), contents.size());
    return path;
}

/*
    Returns the contents of the intervals file that run() wrote for midiPath
*/
string readIntervals(const string& midiPath) {
    ifstream in(midiPath + "_intervals.txt");
    stringstream contents;
    contents << in.rdbuf();
    return contents.str();
}

const bytes endOfTrack = {0x00, 0xff, 0x2f, 0x00};

/*
    Joins event byte lists into one track
*/
bytes track(initializer_list<bytes> events) {
    bytes out;
    for (const bytes& event : events) {
        out.insert(out.end(), event.begin(), event.end());
    }
    return out;
}

TEST(basicTest, CountingTwoTriads) {
    EXPECT_EQ(6, run(folder + "2Triads.mid"));
}

TEST(basicTest, CountingEightNotes) {
    EXPECT_EQ(8, run(folder + "8Notes.mid"));
    EXPECT_EQ("0 0 0 2 9 3 2 ", readIntervals(folder + "8Notes.mid"));
}

TEST(basicTest, noNoteMidiFile) {
    EXPECT_EQ(0, run(folder + "emptySong.mid"));
    EXPECT_EQ("", readIntervals(folder + "emptySong.mid"));
}

TEST(resultsTest, BeethovenMvtI) {
    EXPECT_EQ(22252, run(folderBeethoven +"ScoreMvtI.mid"));
}

TEST(resultsTest, BeethovenMvtII) {
    EXPECT_EQ(9135, run(folderBeethoven +"ScoreMvtII.mid"));
}

TEST(resultsTest, BeethovenMvtIII) {
    EXPECT_EQ(7186, run(folderBeethoven +"ScoreMvtIII.mid"));
}

TEST(resultsTest, BeethovenMvtIV) {
    EXPECT_EQ(14750, run(folderBeethoven +"ScoreMvtIV.mid"));
}

TEST(resultsTest, MozartAria) {
    EXPECT_EQ(1123, run(folderMozart +"MF17-ArieScore.mid"));
}

TEST(resultsTest, BytesAfterEndOfFileAreIgnored) {
    //some old files pad the end with 1a bytes after the last end of track; this copy of Mvt I was edited to match
    run(folderBeethoven + "ScoreMvtI.mid"); //writes the intervals file compared against below
    EXPECT_EQ(22252, run(folderBeethoven +"ScoreMvtIwithWeirdEnding.mid"));
    EXPECT_EQ(readIntervals(folderBeethoven + "ScoreMvtI.mid"), readIntervals(folderBeethoven + "ScoreMvtIwithWeirdEnding.mid"));
}

TEST(intervalTest, NotesAreReducedToPitchClassBeforeTakingTheDistance) {
    //B3 C4 B3 C4 C#4: the semitones across C come out as 11, the one above C as 1 (see interval::interval)
    string path = writeTestFile("semitones.mid", midiFile({track({
        {0x00, 0x90, 59, 0x40}, {0x00, 60, 0x40}, {0x00, 59, 0x40}, {0x00, 60, 0x40}, {0x00, 61, 0x40}, endOfTrack})}));
    EXPECT_EQ(5, run(path));
    EXPECT_EQ("11 11 11 1 ", readIntervals(path));
}

TEST(intervalTest, OctaveIsZero) {
    string path = writeTestFile("octave.mid", midiFile({track({{0x00, 0x90, 60, 0x40}, {0x00, 0x90, 72, 0x40}, endOfTrack})}));
    EXPECT_EQ(2, run(path));
    EXPECT_EQ("0 ", readIntervals(path));
}

TEST(parsingTest, NoteOnWithZeroVelocityIsANoteOff) {
    string path = writeTestFile("noteOff.mid", midiFile({track({
        {0x00, 0x90, 60, 0x40}, {0x60, 0x90, 60, 0x00}, {0x00, 0x80, 62, 0x40}, {0x00, 0x90, 64, 0x40}, endOfTrack})}));
    EXPECT_EQ(2, run(path));
    EXPECT_EQ("4 ", readIntervals(path));
}

TEST(parsingTest, RunningStatusNoteOn) {
    string path = writeTestFile("runningNoteOn.mid", midiFile({track({{0x00, 0x90, 60, 0x40}, {0x00, 62, 0x40}, {0x00, 64, 0x40}, endOfTrack})}));
    EXPECT_EQ(3, run(path));
    EXPECT_EQ("2 2 ", readIntervals(path));
}

TEST(parsingTest, RunningStatusProgramChange) {
    //c0 takes one data byte, so the repeated program change (00 06) is only 2 bytes long
    string path = writeTestFile("runningProgram.mid", midiFile({track({
        {0x00, 0xc0, 0x05}, {0x00, 0x06}, {0x00, 0x90, 60, 0x40}, {0x00, 62, 0x40}, endOfTrack})}));
    EXPECT_EQ(2, run(path));
    EXPECT_EQ("2 ", readIntervals(path));
}

TEST(parsingTest, RunningStatusChannelPressure) {
    string path = writeTestFile("runningPressure.mid", midiFile({track({
        {0x00, 0xd0, 0x10}, {0x00, 0x20}, {0x00, 0x90, 60, 0x40}, {0x00, 67, 0x40}, endOfTrack})}));
    EXPECT_EQ(2, run(path));
    EXPECT_EQ("7 ", readIntervals(path));
}

TEST(parsingTest, MetaAndSysexEventsAreSkipped) {
    string path = writeTestFile("metaSysex.mid", midiFile({track({
        {0x00, 0xff, 0x03, 0x04, 'P', 'i', 'a', 'n'}, //track name
        {0x00, 0xf0, 0x03, 0x7e, 0x09, 0xf7}, //sysex
        {0x00, 0x90, 60, 0x40}, {0x00, 0xb0, 0x07, 0x64}, {0x00, 0x90, 65, 0x40}, endOfTrack})}));
    EXPECT_EQ(2, run(path));
    EXPECT_EQ("5 ", readIntervals(path));
}

TEST(parsingTest, TracksAreJoinedInFileOrder) {
    string path = writeTestFile("twoTracks.mid", midiFile({
        track({{0x00, 0x90, 60, 0x40}, {0x00, 64, 0x40}, endOfTrack}),
        track({{0x00, 0x91, 67, 0x40}, endOfTrack})}));
    EXPECT_EQ(3, run(path));
    EXPECT_EQ("4 3 ", readIntervals(path));
}

TEST(parsingTest, LongerHeaderIsSkipped) {
    //an MThd length above 6 is allowed, and readers should skip the extra bytes
    bytes file = midiFile({track({{0x00, 0x90, 60, 0x40}, {0x00, 63, 0x40}, endOfTrack})});
    file[7] = 10; //read as hex (0000000a), not decimal
    file.insert(file.begin() + 14, {0x00, 0x00, 0x00, 0x00});
    string path = writeTestFile("longHeader.mid", file);
    EXPECT_EQ(2, run(path));
    EXPECT_EQ("3 ", readIntervals(path));
}

TEST(errorTest, MissingFileThrows) {
    EXPECT_THROW(run(folder + "doesNotExist.mid"), runtime_error);
}

TEST(errorTest, EmptyFileThrows) {
    EXPECT_THROW(run(writeTestFile("zeroBytes.mid", {})), runtime_error);
}

TEST(errorTest, NonMidiFileThrows) {
    string text = "hello, this is not a MIDI file";
    EXPECT_THROW(run(writeTestFile("notMidi.mid", bytes(text.begin(), text.end()))), runtime_error);
}

TEST(errorTest, TruncatedFileThrows) {
    bytes file = midiFile({track({{0x00, 0x90, 60, 0x40}, {0x00, 62, 0x40}, {0x00, 64, 0x40}, endOfTrack})});
    file.resize(file.size() - 6); //cut off partway through the last note
    EXPECT_THROW(run(writeTestFile("truncated.mid", file)), runtime_error);
}

int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
