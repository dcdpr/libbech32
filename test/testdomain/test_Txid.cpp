#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "txid.h"
#include "../mock_bitcoinRPCFacade.h"

using ::testing::Return;
using ::testing::_;


TEST(TxidTest, empty_txids_are_not_valid) {
    MockBitcoinRPCFacade btc;
    ASSERT_THROW(std::unique_ptr<Txid>(new Txid("", btc)), std::runtime_error);
}

TEST(TxidTest, txids_of_incorrect_length_are_not_valid) {
    MockBitcoinRPCFacade btc;

    std::string sixtyThreeChars = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c";
    ASSERT_THROW(std::unique_ptr<Txid>(new Txid(sixtyThreeChars, btc)), std::runtime_error);

    std::string sixtyFiveChars = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5a";
    ASSERT_THROW(std::unique_ptr<Txid>(new Txid(sixtyFiveChars, btc)), std::runtime_error);
}

TEST(TxidTest, txids_with_invalid_characters_are_not_valid) {
    MockBitcoinRPCFacade btc;

    std::string someInvalidChars = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282ch";
    ASSERT_THROW(std::unique_ptr<Txid>(new Txid(someInvalidChars, btc)), std::runtime_error);

    std::string someOtherInvalidChars = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c$";
    ASSERT_THROW(std::unique_ptr<Txid>(new Txid(someOtherInvalidChars, btc)), std::runtime_error);
}

TEST(TxidTest, txid_not_found) {
    MockBitcoinRPCFacade btc;

    // if bitcoind CAN'T find a txid, it will return nothing for the hex of the rawtransaction
    getrawtransaction_t rawTransaction;
    rawTransaction.hex = "";

    EXPECT_CALL(btc, getrawtransaction(_,_))
            .WillOnce(Return(rawTransaction));

    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";
    ASSERT_THROW(std::unique_ptr<Txid>(new Txid(txidStr, btc)), std::runtime_error);
}


TEST(TxidTest, txid_is_found) {
    MockBitcoinRPCFacade btc;

    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillOnce(Return(rawTransaction1));

    // it will also return a blockhash
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillOnce(Return(rawTransaction2));

    blockchaininfo_t blockChainInfo;
    blockChainInfo.chain = "test";
    EXPECT_CALL(btc, getblockchaininfo())
            .WillOnce(Return(blockChainInfo));

    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = 12345;
    std::vector<std::string> blockTransactions {"123abc", txidStr, "234abc"};
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillOnce(Return(blockInfo));

    std::unique_ptr<Txid> txid;

    ASSERT_NO_THROW(txid.reset(new Txid(txidStr, btc)));

    ASSERT_EQ(blockInfo.height, txid->blockHeight()->value());
    ASSERT_EQ(1, txid->transactionPosition()->value());
}


TEST(TxidTest, test_txid_equality) {
    MockBitcoinRPCFacade btc;

    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillOnce(Return(rawTransaction1));

    // it will also return a blockhash
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillOnce(Return(rawTransaction2));

    blockchaininfo_t blockChainInfo;
    blockChainInfo.chain = "test";
    EXPECT_CALL(btc, getblockchaininfo())
            .WillOnce(Return(blockChainInfo));

    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = 12345;
    std::vector<std::string> blockTransactions {"123abc", txidStr, "234abc"};
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillOnce(Return(blockInfo));

    /// ....
    std::unique_ptr<Txid> txid;

    ASSERT_NO_THROW(txid.reset(new Txid(txidStr, btc)));

    ASSERT_EQ(blockInfo.height, txid->blockHeight()->value());
    ASSERT_EQ(1, txid->transactionPosition()->value());
}

