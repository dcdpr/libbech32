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

    // this test should throw because the above MockBitcoinRPCFacade was not set up
    // so that this txidStr would be found
    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";
    ASSERT_THROW(std::unique_ptr<Txid>(new Txid(txidStr, btc)), std::runtime_error);
}

/**
 * Convenience function to return a lowercase string
 * @param s the input string
 * @return the lowercase string
 */
std::string lowercase(const std::string &s) {
    std::string t = s;
    transform(t.begin(), t.end(), t.begin(), &tolower);
    return t;
}

/**
 * Convenience function to create a Txid object for testing.
 *
 * Will create and use a MockBitcoinRPCFacade which will associate the
 * txidStr with the given blockHeight and transactionPosition. These need not
 * be truly valid.
 *
 * @param txidStr The txid string to use
 * @param blockHeight The block height
 * @param transactionPosition The transaction position
 * @return A dumb pointer to a new Txid object
 */
Txid * createTestTxid(
        const std::string &txidStr,
        int blockHeight,
        int transactionPosition)
{
    MockBitcoinRPCFacade btc;

    assert(blockHeight >= 0);
    assert(transactionPosition >= 0);

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

    // and a block height and collection of transaction ids
    blockinfo_t blockInfo;
    blockInfo.height = blockHeight;
    std::vector<std::string> blockTransactions {"123abc", "234abc", "456abc"};
    blockTransactions[
            static_cast<std::vector<std::string>::size_type>(transactionPosition)] = lowercase(txidStr);
    blockInfo.tx = blockTransactions;
    EXPECT_CALL(btc, getblock(_))
            .WillOnce(Return(blockInfo));

    return new Txid(txidStr, btc);
}

TEST(TxidTest, txid_is_found) {
    int blockHeight = 12345;
    int transactionPosition = 1;
    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    std::unique_ptr<Txid> txid;

    ASSERT_NO_THROW(txid.reset(createTestTxid(txidStr, blockHeight, transactionPosition)));

    ASSERT_EQ(blockHeight, txid->blockHeight()->value());
    ASSERT_EQ(transactionPosition, txid->transactionPosition()->value());
}

TEST(TxidTest, txid_with_uppercase_chars_is_found) {
    int blockHeight = 12345;
    int transactionPosition = 1;
    std::string txidStr = "8A76B282FA1E3585D5C4C0DD2774400AA0A075E2CD255F0F5324F2E837F282C5";

    std::unique_ptr<Txid> txid;

    ASSERT_NO_THROW(txid.reset(createTestTxid(txidStr, blockHeight, transactionPosition)));

    ASSERT_EQ(blockHeight, txid->blockHeight()->value());
    ASSERT_EQ(transactionPosition, txid->transactionPosition()->value());
}

TEST(TxidTest, test_txid_equality) {
    int blockHeight = 12345;
    int transactionPosition = 1;
    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    std::unique_ptr<Txid> txid1;
    ASSERT_NO_THROW(txid1.reset(createTestTxid(txidStr, blockHeight, transactionPosition)));

    txidStr = "8A76B282FA1E3585D5C4C0DD2774400AA0A075E2CD255F0F5324F2E837F282C5";

    std::unique_ptr<Txid> txid2;
    ASSERT_NO_THROW(txid2.reset(createTestTxid(txidStr, blockHeight, transactionPosition)));

    ASSERT_EQ(*txid1, *txid2);
}

TEST(TxidTest, test_txid_inequality_for_diff_blockheight) {
    int blockHeight = 12345;
    int transactionPosition = 1;
    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    std::unique_ptr<Txid> txid1;
    ASSERT_NO_THROW(txid1.reset(createTestTxid(txidStr, blockHeight, transactionPosition)));

    txidStr = "8A76B282FA1E3585D5C4C0DD2774400AA0A075E2CD255F0F5324F2E837F282C5";
    blockHeight = 12344;

    std::unique_ptr<Txid> txid2;
    ASSERT_NO_THROW(txid2.reset(createTestTxid(txidStr, blockHeight, transactionPosition)));

    ASSERT_NE(*txid1, *txid2);
}

TEST(TxidTest, test_txid_inequality_for_diff_txpos) {
    int blockHeight = 12345;
    int transactionPosition = 1;
    std::string txidStr = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    std::unique_ptr<Txid> txid1;
    ASSERT_NO_THROW(txid1.reset(createTestTxid(txidStr, blockHeight, transactionPosition)));

    txidStr = "8A76B282FA1E3585D5C4C0DD2774400AA0A075E2CD255F0F5324F2E837F282C5";
    transactionPosition = 2;

    std::unique_ptr<Txid> txid2;
    ASSERT_NO_THROW(txid2.reset(createTestTxid(txidStr, blockHeight, transactionPosition)));

    ASSERT_NE(*txid1, *txid2);
}

