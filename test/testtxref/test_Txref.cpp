#include <gtest/gtest.h>

#include "test_Txref.h"
#include "txref.cpp"

// check that we accept block heights within the correct range for both main and testnet
TEST_F(TxrefTest, accept_good_block_heights) {
    EXPECT_NO_THROW(checkBlockHeightRange(0));
    EXPECT_NO_THROW(checkBlockHeightRange(1));
    EXPECT_NO_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT));

    EXPECT_NO_THROW(checkBlockHeightRangeTestnet(0));
    EXPECT_NO_THROW(checkBlockHeightRangeTestnet(1));
    EXPECT_NO_THROW(checkBlockHeightRangeTestnet(MAX_BLOCK_HEIGHT_TESTNET));
}

// check that we reject block heights outside of the range for both main and testnet
TEST_F(TxrefTest, reject_bad_block_heights) {
    EXPECT_THROW(checkBlockHeightRange(-1), std::runtime_error);
    EXPECT_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT + 1), std::runtime_error);

    EXPECT_THROW(checkBlockHeightRangeTestnet(-1), std::runtime_error);
    EXPECT_THROW(checkBlockHeightRangeTestnet(MAX_BLOCK_HEIGHT_TESTNET + 1), std::runtime_error);
}

// check that we accept transaction positions within the correct range for both main and testnet
TEST_F(TxrefTest, accept_good_transaction_position) {
    EXPECT_NO_THROW(checkTransactionPositionRange(0));
    EXPECT_NO_THROW(checkTransactionPositionRange(1));
    EXPECT_NO_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION));

    EXPECT_NO_THROW(checkTransactionPositionRangeTestnet(0));
    EXPECT_NO_THROW(checkTransactionPositionRangeTestnet(1));
    EXPECT_NO_THROW(checkTransactionPositionRangeTestnet(MAX_TRANSACTION_POSITION_TESTNET));
}

// check that we reject transaction positions outside of the range for both main and testnet
TEST_F(TxrefTest, reject_bad_transaction_position) {

    EXPECT_THROW(checkTransactionPositionRange(-1), std::runtime_error);
    EXPECT_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION + 1), std::runtime_error);

    EXPECT_THROW(checkTransactionPositionRangeTestnet(-1), std::runtime_error);
    EXPECT_THROW(checkTransactionPositionRangeTestnet(MAX_TRANSACTION_POSITION_TESTNET + 1), std::runtime_error);
}

// check that we accept magic codes within the correct range
TEST_F(TxrefTest, accept_good_magic_code) {
    EXPECT_NO_THROW(checkMagicCodeRange(0));
    EXPECT_NO_THROW(checkMagicCodeRange(1));
    EXPECT_NO_THROW(checkMagicCodeRange(MAX_MAGIC_CODE));
}

// check that we accept magic codes outside of the range
TEST_F(TxrefTest, reject_bad_magic_code) {
    EXPECT_THROW(checkMagicCodeRange(-1), std::runtime_error);
    EXPECT_THROW(checkMagicCodeRange(MAX_MAGIC_CODE + 1), std::runtime_error);
}

// check that we correctly encode txrefs
TEST_F(TxrefTest, txref_encode_mainnet) {
    EXPECT_EQ(txref::bitcoinTxrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0),
              "tx1-rqqq-qqqq-qmhu-qk");
    EXPECT_EQ(txref::bitcoinTxrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x1FFF),
              "tx1-rqqq-qull-6v87-r7");
    EXPECT_EQ(txref::bitcoinTxrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x1FFFFF, 0),
              "tx1-r7ll-lrqq-vq5e-gg");
    EXPECT_EQ(txref::bitcoinTxrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x1FFFFF, 0x1FFF),
              "tx1-r7ll-llll-khym-tq");
    EXPECT_EQ(txref::bitcoinTxrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205),
              "tx1-rjk0-u5ng-4jsf-mc");
}

// check that we correctly encode txrefs for testnet
TEST_F(TxrefTest, txref_encode_testnet) {
    EXPECT_EQ(txref::bitcoinTxrefEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0),
              "txtest1-xqqq-qqqq-qqkn-3gh9");
    EXPECT_EQ(txref::bitcoinTxrefEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0x3FFFFFF, 0x3FFFF),
              "txtest1-x7ll-llll-llj9-t9dk");
    EXPECT_EQ(txref::bitcoinTxrefEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 467883, 2355),
              "txtest1-xk63-uqvx-fqx8-xqr8");
}

// check that we can extract the block height from txrefs for both main and testnet
TEST_F(TxrefTest, extract_block_height) {
    std::string txref;
    int blockHeight;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqmhuqk";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1rqqqqull6v87r7";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1r7lllrqqvq5egg";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0x1FFFFF);

    txref = "tx1r7llllllkhymtq";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0x1FFFFF);

    txref = "tx1rjk0u5ng4jsfmc";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

    txref = "txtest1xqqqqqqqqqkn3gh9";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "txtest1x7llllllllj9t9dk";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0x3FFFFFF);

    txref = "txtest1xk63uqvxfqx8xqr8";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 467883);
}

// check that we can extract the transaction position from txrefs for both main and testnet
TEST_F(TxrefTest, extract_transaction_position) {
    std::string txref;
    int transactionPosition;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqmhuqk";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1rqqqqull6v87r7";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x1FFF);

    txref = "tx1r7lllrqqvq5egg";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1r7llllllkhymtq";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x1FFF);

    txref = "tx1rjk0u5ng4jsfmc";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);

    txref = "txtest1xqqqqqqqqqkn3gh9";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "txtest1x7llllllllj9t9dk";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x3FFFF);

    txref = "txtest1xk63uqvxfqx8xqr8";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2355);
}

//check that we correctly decode txrefs for both main and testnet
TEST_F(TxrefTest, txref_decode) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1-rqqq-qqqq-qmhu-qk";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "tx1-rqqq-qull-6v87-r7";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0x1FFF);

    txref = "tx1-r7ll-lrqq-vq5e-gg";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0x1FFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "tx1-r7ll-llll-khym-tq";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0x1FFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x1FFF);

    txref = "tx1-rjk0-u5ng-4jsf-mc";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);


    txref = "txtest1-xqqq-qqqq-qqkn-3gh9";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "txtest1-x7ll-llll-llj9-t9dk";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0x3FFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x3FFFF);

    txref = "txtest1-xk63-uqvx-fqx8-xqr8";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 467883);
    EXPECT_EQ(loc.transactionPosition, 2355);

}

// check that we can deal with weird txref formatting (invalid characters)
TEST_F(TxrefTest, txref_decode_weird_formatting) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1---rqqq-<qqqq>-q>m<hu-q##k--";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "tx1-rqqq qqqq qmhu qk";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "tx1rqqq,qqqq.qmhu.qk";
    loc = txref::bitcoinTxrefDecode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);

}
