#include <gtest/gtest.h>
#include <gmock/gmock.h>

// TODO find a better way to include objects from main src directory
#include "../../src/bitcoinRPCFacade.cpp"
#include "../../src/classifyInputString.cpp"
#include "txid.cpp"
#include "vout.cpp"
#include "blockHeight.cpp"
#include "transactionPosition.cpp"
#include "txref.cpp"
#include "did.cpp"

int main(int argc, char **argv) {
    ::testing::InitGoogleMock(&argc, argv);

    return RUN_ALL_TESTS();
}
