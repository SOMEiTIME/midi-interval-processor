#include "midiToInterval.h"
#include "musicalConcept.h"
#include <gtest/gtest.h>

using namespace std;

/*
    Tests for use with google test
*/
string folder = "testData/"; 
string folderBeethoven = folder + "BeethovenSymphony3-mids/";
string folderMozart = folder + "magicflute-17-aria-mids/";
string folderBadMidi = "badMidi/";

//When testing, consider using an allowed range, as some files, in some formats have a different note count
//between my method and whatever method Logic X uses to count notes
//note: Logic X and Finale 2014.5 always differ on note counts due to different definitions of notes

TEST(basicTest, CountingTwoTriads) {
    EXPECT_EQ(6, run(folder + "2Triads.mid"));
}

TEST(basicTest, CountingEightNotes) {
    EXPECT_EQ(8, run(folder + "8Notes.mid"));
}

TEST(basicTest, noNoteMidiFile) {
    EXPECT_EQ(0, run(folder + "emptySong.mid"));
}

TEST(resultsTest, FullPieceTest) {
    EXPECT_EQ(22252, run(folderBeethoven +"ScoreMvtI.mid"));
}

TEST(resultsTest, FullPieceTest2) {
    EXPECT_EQ(9135, run(folderBeethoven +"ScoreMvtII.mid"));
}

TEST(resultsTest, FullPieceTest3) {
    EXPECT_EQ(7186, run(folderBeethoven +"ScoreMvtIII.mid"));
}

TEST(resultsTest, FullPieceTest4) {
    EXPECT_EQ(14750, run(folderBeethoven +"ScoreMvtIV.mid"));
}

TEST(resultsTest, FullPieceTest5) {
    EXPECT_EQ(1123, run(folderMozart +"MF17-ArieScore.mid"));
}

TEST(resultsTest, problemFile1) {
    EXPECT_EQ(22252, run(folderBeethoven +"ScoreMvtIwithWeirdEnding.mid")); //some old files were found to have a string of 1a1a after the end of file marker, 
    //this file was made in that style
}



int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}

      
