#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "txref.h"

#include "vout.h"
#include "txid.h"
#include "../mock_bitcoinRPCFacade.h"

using ::testing::Return;
using ::testing::_;

TEST(TxrefTest, constructingTxref_withEmptyTxref_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    std::string txrefStr;

    std::unique_ptr<Txref> txrefp;
    ASSERT_THROW(txrefp.reset(new Txref(txrefStr, btc)), std::runtime_error);

}

TEST(TxrefTest, constructingTxref_withBadTxref_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    std::string txrefStr = "xg4h-jzgq-pqpq-q9r6sd9";

    std::unique_ptr<Txref> txrefp;
    ASSERT_THROW(txrefp.reset(new Txref(txrefStr, btc)), std::runtime_error);

}

TEST(TxrefTest, constructingTxref_withGoodTxidAndVout_isSuccessful) {
    MockBitcoinRPCFacade btc;

    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillOnce(Return(rawTransaction1));

    // it will also return a blockhash
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = "3243f6a8885a308d";
    // and a vector of vouts
    rawTransaction2.vout.emplace_back();
    rawTransaction2.vout.emplace_back();
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillRepeatedly(Return(rawTransaction2));

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

    // we made two vouts in the mock above, let's pick the second one
    Vout vout(1);

    std::unique_ptr<Txref> txrefp;
    ASSERT_NO_THROW(txrefp.reset(new Txref(*txid, vout, btc)));

}

TEST(TxrefTest, constructingTxref_withGoodTxidAndBadVout_isUnsuccessful) {
    MockBitcoinRPCFacade btc;

    // if bitcoind CAN find a txid, it will return the hex of the rawtransaction
    getrawtransaction_t rawTransaction1;
    rawTransaction1.hex = "3243f6a8885a308d";
    EXPECT_CALL(btc, getrawtransaction(_,0))
            .WillOnce(Return(rawTransaction1));

    // it will also return a blockhash
    getrawtransaction_t rawTransaction2;
    rawTransaction2.blockhash = "3243f6a8885a308d";
    // and a vector of vouts
    rawTransaction2.vout.emplace_back();
    rawTransaction2.vout.emplace_back();
    EXPECT_CALL(btc, getrawtransaction(_,1))
            .WillRepeatedly(Return(rawTransaction2));

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

    // we made two vouts in the mock above, attempt to reference too many vouts
    Vout vout(10);

    std::unique_ptr<Txref> txrefp;
    ASSERT_THROW(txrefp.reset(new Txref(*txid, vout, btc)), std::runtime_error);

}

TEST(TxrefTest, constructingTxref_withGoodTxref_isSuccessful) {
    MockBitcoinRPCFacade btc;

    // this txrefStr was made via
    // txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205)
    std::string txrefStr = "rjk0-uqay-zsrw-hqe";
    int blockHeight = 466793;
    std::vector<std::string>::size_type transactionPos = 2205;
    std::string txidStr = "0c92d46fb3d9dfd60bead61515f0a13159f77c9800a9e572f1b64e1ba2dc24f6";

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


    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = blockHeight;
    std::vector<std::string> blockTransactions(transactionPos+1);
    blockTransactions[transactionPos] = txidStr;
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillRepeatedly(Return(blockInfo));

    Txid expectedTxid(txidStr, btc);
    Vout expectedVout(0);

    // test that we can construct the txref
    std::unique_ptr<Txref> txrefp;
    ASSERT_NO_THROW(txrefp.reset(new Txref(txrefStr, btc)));

    // test that the txref's txids and vouts are equal to what we expect
    ASSERT_EQ(expectedTxid, *txrefp->getTxid());
    ASSERT_EQ(expectedVout, *txrefp->getVout());

}

TEST(TxrefTest, constructingTxref_withGoodTxrefExt_isSuccessful) {
    MockBitcoinRPCFacade btc;

    // this txrefStr was made via
    // txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 0)
    std::string txrefStr = "yqqq-qqqq-qqqq-ksvh-26";
    int blockHeight = 0;
    std::vector<std::string>::size_type transactionPos = 0;
    std::string txidStr = "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b";


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

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = blockHeight;
    std::vector<std::string> blockTransactions(transactionPos+1);
    blockTransactions[transactionPos] = txidStr;
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillRepeatedly(Return(blockInfo));

    Txid expectedTxid(txidStr, btc);
    Vout expectedVout(0);

    // test that we can construct the txref
    std::unique_ptr<Txref> txrefp;
    ASSERT_NO_THROW(txrefp.reset(new Txref(txrefStr, btc)));

    // test that the txref's txids and vouts are equal to what we expect
    ASSERT_EQ(expectedTxid, *txrefp->getTxid());
    ASSERT_EQ(expectedVout, *txrefp->getVout());

}

TEST(TxrefTest, constructingTxref_withGoodTxrefExtVout1_isSuccessful) {
    MockBitcoinRPCFacade btc;

    // this txrefStr was made via
    // txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 1355601, 1022, 1)
    std::string txrefStr = "8z4h-jz7l-qpqq-n28t-rl";
    int blockHeight = 1355601;
    std::vector<std::string>::size_type transactionPos = 1022;
    std::string txidStr = "cb0252c5ea4e24bee19edd1ed1338ef077dc75d30383097d8c4bae3a9862b35a";


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

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = blockHeight;
    std::vector<std::string> blockTransactions(transactionPos+1);
    blockTransactions[transactionPos] = txidStr;
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillRepeatedly(Return(blockInfo));

    Txid expectedTxid(txidStr, btc);
    Vout expectedVout(1);

    // test that we can construct the txref
    std::unique_ptr<Txref> txrefp;
    ASSERT_NO_THROW(txrefp.reset(new Txref(txrefStr, btc)));

    // test that the txref's txids and vouts are equal to what we expect
    ASSERT_EQ(expectedTxid, *txrefp->getTxid());
    ASSERT_EQ(expectedVout, *txrefp->getVout());

}
