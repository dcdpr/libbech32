#include <gtest/gtest.h>

#include "classifyInputString.h"
#include "classifyInputString.cpp"


TEST(ClassifyInputStringTest, test_empty) {
    EXPECT_EQ(classifyInputString(""), unknown_param);
}

TEST(ClassifyInputStringTest, test_random) {
    EXPECT_EQ(classifyInputString("oihjediouhwisdubch"), unknown_param);
}

TEST(ClassifyInputStringTest, test_address) {
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeFc3iqRYhem"), address_param);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZifT9YgRrkSgzQX"), address_param);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47XtaisrJa1Vc"), address_param);
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2QNz2MQ3Se"), address_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYkc3QYZ4"), address_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYk8ec3QYZ4"), unknown_param);
}

TEST(ClassifyInputStringTest, test_bad_address) {
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeFc3iqRYhemse"), unknown_param);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZifT9YgRrkSgzQXdd"), unknown_param);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47XtaisrJa1Vcd"), unknown_param);
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2QNz2MQ3Sedd"), unknown_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYkc3QYZ4dd"), unknown_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYk8ec3QYZ4"), unknown_param);
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeF"), unknown_param);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZffT9Y"), unknown_param);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47X"), unknown_param);
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2"), unknown_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfa"), unknown_param);
}

TEST(ClassifyInputStringTest, test_txid) {
    EXPECT_EQ(classifyInputString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"), txid_param);
    EXPECT_EQ(classifyInputString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca4953991b7852b855"), unknown_param);
}

TEST(ClassifyInputStringTest, test_txref) {
    // mainnet
    EXPECT_EQ(classifyInputString("tx1rqqqqqqqqmhuqk"), txref_param);
    EXPECT_EQ(classifyInputString("rqqqqqqqqmhuqk"), txref_param);
    // testnet
    EXPECT_EQ(classifyInputString("txtest1xk63uqvxfqx8xqr8"), txref_param);
    EXPECT_EQ(classifyInputString("xk63uqvxfqx8xqr8"), txref_param);
}

TEST(ClassifyInputStringTest, test_txrefext) {
    // mainnet
    EXPECT_EQ(classifyInputString("tx1rqqqqqqqqqquau7hl"), txrefext_param);
    EXPECT_EQ(classifyInputString("rqqqqqqqqqquau7hl"), txrefext_param);
    // testnet
    EXPECT_EQ(classifyInputString("txtest1xqqqqqqqqqyrqtc39q4"), txrefext_param);
    EXPECT_EQ(classifyInputString("xqqqqqqqqqyrqtc39q4"), txrefext_param);
}

