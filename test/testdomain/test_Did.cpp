#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "did.h"

#include "../mock_bitcoinRPCFacade.h"

using ::testing::Return;
using ::testing::_;


TEST(DidTest, constructingDid_withEmptyDidStr_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    std::string didStr;

    std::unique_ptr<Did> pDid;
    ASSERT_THROW(pDid.reset(new Did(didStr, btc)), std::runtime_error);

}

TEST(DidTest, constructingDid_withBadDidStr_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    std::string didStr = "foobar";

    std::unique_ptr<Did> pDid;
    ASSERT_THROW(pDid.reset(new Did(didStr, btc)), std::runtime_error);

}

TEST(DidTest, constructingDid_withBadTxrefStr_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    std::string didStr = "did:btcr:xz4h-jzcl-foobar";

    std::unique_ptr<Did> pDid;
    ASSERT_THROW(pDid.reset(new Did(didStr, btc)), std::runtime_error);

}

TEST(DidTest, constructingDid_withGoodTxrefStr_isSuccessful) {
    MockBitcoinRPCFacade btc;

    std::string didStr = "did:btcr:rjk0-uqay-zsrw-hqe";

    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillRepeatedly(Return(rawTransaction1));

    // it will also return a blockhash
    std::string fakeBlockhash = "3243f6a8885a308d";
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = fakeBlockhash;
    // and a vector of vouts
    rawTransaction2.vout.emplace_back();
    rawTransaction2.vout.emplace_back();
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillRepeatedly(Return(rawTransaction2));

    EXPECT_CALL(btc, getblockhash(_))
            .WillRepeatedly(Return(fakeBlockhash));

    blockchaininfo_t blockChainInfo;
    blockChainInfo.chain = "test";
    EXPECT_CALL(btc, getblockchaininfo())
            .WillRepeatedly(Return(blockChainInfo));

    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";
    int blockHeight = 466793;
    std::vector<std::string>::size_type transactionPos = 2205;

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = blockHeight;
    std::vector<std::string> blockTransactions(transactionPos+1);
    blockTransactions[transactionPos] = txidStr;
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillRepeatedly(Return(blockInfo));


    std::unique_ptr<Did> pDid;
    ASSERT_NO_THROW(pDid.reset(new Did(didStr, btc)));

}

// TODO: redo above test with an extended txref with a non-zero Vout
