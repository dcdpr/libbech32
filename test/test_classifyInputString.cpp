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
    // too long
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeFc3iqRYhemse"), unknown_param);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZifT9YgRrkSgzQXdd"), unknown_param);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47XtaisrJa1Vcd"), unknown_param);
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2QNz2MQ3Sedd"), unknown_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYkc3QYZ4dd"), unknown_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfahYk8ec3QYZ4"), unknown_param);
    // too short
    EXPECT_EQ(classifyInputString("17VZNX1SN5NtKa8UQFxwQbFeF"), unknown_param);
    EXPECT_EQ(classifyInputString("3EktnHQD7RiAE6uzMj2ZffT9Y"), unknown_param);
    EXPECT_EQ(classifyInputString("2MzQwSSnBHWHqSAqtTVQ6v47X"), unknown_param);
}

TEST(ClassifyInputStringTest, test_anomolies) {
    // classifyInputString isn't perfect. Here are some examples where it is wrong

    // should be "unknown_param" since these are too-short bitcoin addresses, but they happen
    // to have the right number of characters after being cleaned of invalid characters
    EXPECT_EQ(classifyInputString("mzgjzyj9i9JyU5zBQyNBZMkm2"), txref_param);
    EXPECT_EQ(classifyInputString("mxgj4vFNNWPdRb45tHoJoVqfa"), txref_param);
}

TEST(ClassifyInputStringTest, test_txid) {
    EXPECT_EQ(classifyInputString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"), txid_param);
    EXPECT_EQ(classifyInputString("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca4953991b7852b855"), unknown_param);
}

TEST(ClassifyInputStringTest, test_txref) {
    // mainnet
    EXPECT_EQ(classifyInputString("tx1rqqqqqqqqmhuqhp"), txref_param);
    EXPECT_EQ(classifyInputString("rqqqqqqqqmhuqhp"), txref_param);
    // testnet
    EXPECT_EQ(classifyInputString("txtest1xjk0uqayzat0dz8"), txref_param);
    EXPECT_EQ(classifyInputString("xjk0uqayzat0dz8"), txref_param);
}

TEST(ClassifyInputStringTest, test_txrefext) {
    // mainnet
    EXPECT_EQ(classifyInputString("tx1yqqqqqqqqqqqksvh26"), txrefext_param);
    EXPECT_EQ(classifyInputString("yqqqqqqqqqqqksvh26"), txrefext_param);
    // testnet
    EXPECT_EQ(classifyInputString("txtest18jk0uqayzu4xaw4hzl"), txrefext_param);
    EXPECT_EQ(classifyInputString("8jk0uqayzu4xaw4hzl"), txrefext_param);
}

