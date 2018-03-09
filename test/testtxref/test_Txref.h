#ifndef TXREF_TEST_TXREF_H
#define TXREF_TEST_TXREF_H

#include <gtest/gtest.h>

class TxrefTest : public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    TxrefTest() = default;

    // You can do clean-up work that doesn't throw exceptions here.
    ~TxrefTest() override = default;

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    void SetUp() override {
        Test::SetUp();
    }

    // Code here will be called immediately after each test (right
    // before the destructor).
    void TearDown() override {
        Test::TearDown();
    }

};

#endif //TXREF_TEST_TXREF_H
