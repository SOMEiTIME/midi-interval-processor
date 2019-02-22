#include "midi_to_interval.h"
#include <gtest/gtest.h>

string folder = "testData/"; 
int allowedRange = 10; //there is an allowed range, as some files, in some formats have a different note count
        //between my method and whatever method Logic X uses to count notes
        //note: Logic X and Finale 2014.5 always differ on note counts due to different definitions of notes

TEST(basicTest, CountingTwoTriads) {
    EXPECT_EQ(6, run(folder + "2Triads.mid"));
}


TEST(basicTest, CountingEightNotes) {
    EXPECT_EQ(8, run(folder + "8Notes.mid"));
}


TEST(resultsTest, FullPieceTest) {
    ASSERT_NEAR(297, run(folder + "testMidi.mid"), allowedRange); 
}

TEST(resultsTest, FullPieceTest2) {
    ASSERT_NEAR(33835, run(folder + "Beethoven.mid"), allowedRange); 
}


TEST(resultsTest, FullPieceTest3) {
    ASSERT_NEAR(8993, run(folder + "Kyrie.mid"), allowedRange); 
}


TEST(resultsTest, FullPieceTest4) {
    ASSERT_NEAR(5930, run(folder + "Symphony.mid"), allowedRange);                                        
}


TEST(resultsTest, noNoteMidiFile) {
    EXPECT_EQ(0, run(folder + "emptySong.mid"));
}


TEST(resultsTest, problemFile1) {
    ASSERT_NEAR(3219, run(folder +"Concerto_Grosso_op6_n05_3mov.mid"), allowedRange);

}

TEST(resultsTest, problemFile1LogicExported) {
    ASSERT_NEAR(3219, run(folder +"logicGrosso.mid"), allowedRange);
}

TEST(resultsTest, problemFile2) {
    ASSERT_NEAR(1362, run(folder +"Harpsichord_Suite_n3_Hwv428_3mov.mid"), allowedRange);
}

int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}

      
