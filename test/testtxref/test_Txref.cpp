#include <gtest/gtest.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <rapidcheck/gtest.h>
#pragma clang diagnostic pop

#include "txref.cpp"

// check that we accept block heights within the correct range for both main and testnet
TEST(TxrefTest, accept_good_block_heights) {
    EXPECT_NO_THROW(checkBlockHeightRange(0));
    EXPECT_NO_THROW(checkBlockHeightRange(1));
    EXPECT_NO_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT));

    EXPECT_NO_THROW(checkBlockHeightRangeTestnet(0));
    EXPECT_NO_THROW(checkBlockHeightRangeTestnet(1));
    EXPECT_NO_THROW(checkBlockHeightRangeTestnet(MAX_BLOCK_HEIGHT_TESTNET));
}

RC_GTEST_PROP(TxrefTestRC, goodBlockHeightsAreAccepted, ()
) {
    // generate valid block heights
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    checkBlockHeightRange(height);

    height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT_TESTNET);
    checkBlockHeightRangeTestnet(height);
}


// check that we reject block heights outside of the range for both main and testnet
TEST(TxrefTest, reject_bad_block_heights) {
    EXPECT_THROW(checkBlockHeightRange(-1), std::runtime_error);
    EXPECT_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT + 1), std::runtime_error);

    EXPECT_THROW(checkBlockHeightRangeTestnet(-1), std::runtime_error);
    EXPECT_THROW(checkBlockHeightRangeTestnet(MAX_BLOCK_HEIGHT_TESTNET + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badBlockHeightsAreRejected, ()
) {
        // generate out of range block heights
    auto height = *rc::gen::inRange(-MAX_BLOCK_HEIGHT, -1);
    RC_ASSERT_THROWS_AS(checkBlockHeightRange(height), std::runtime_error);

    height = *rc::gen::inRange(MAX_BLOCK_HEIGHT+1, 2*MAX_BLOCK_HEIGHT);
    RC_ASSERT_THROWS_AS(checkBlockHeightRange(height), std::runtime_error);

    height = *rc::gen::inRange(-MAX_BLOCK_HEIGHT_TESTNET, -1);
    RC_ASSERT_THROWS_AS(checkBlockHeightRangeTestnet(height), std::runtime_error);

    height = *rc::gen::inRange(MAX_BLOCK_HEIGHT_TESTNET+1, 2*MAX_BLOCK_HEIGHT_TESTNET);
    RC_ASSERT_THROWS_AS(checkBlockHeightRangeTestnet(height), std::runtime_error);
}


// check that we accept transaction positions within the correct range for both main and testnet
TEST(TxrefTest, accept_good_transaction_position) {
    EXPECT_NO_THROW(checkTransactionPositionRange(0));
    EXPECT_NO_THROW(checkTransactionPositionRange(1));
    EXPECT_NO_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION));

    EXPECT_NO_THROW(checkTransactionPositionRangeTestnet(0));
    EXPECT_NO_THROW(checkTransactionPositionRangeTestnet(1));
    EXPECT_NO_THROW(checkTransactionPositionRangeTestnet(MAX_TRANSACTION_POSITION_TESTNET));
}

RC_GTEST_PROP(TxrefTestRC, goodTransactionPositionsAreAccepted, ()
) {
    // generate valid transaction positions
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    checkTransactionPositionRange(pos);

    pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION_TESTNET);
    checkTransactionPositionRangeTestnet(pos);
}


// check that we reject transaction positions outside of the range for both main and testnet
TEST(TxrefTest, reject_bad_transaction_position) {

    EXPECT_THROW(checkTransactionPositionRange(-1), std::runtime_error);
    EXPECT_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION + 1), std::runtime_error);

    EXPECT_THROW(checkTransactionPositionRangeTestnet(-1), std::runtime_error);
    EXPECT_THROW(checkTransactionPositionRangeTestnet(MAX_TRANSACTION_POSITION_TESTNET + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badTransactionPositionsAreRejected, ()
) {
    // generate out of range transaction positions
    auto pos = *rc::gen::inRange(-MAX_TRANSACTION_POSITION, -1);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRange(pos), std::runtime_error);

    pos = *rc::gen::inRange(MAX_TRANSACTION_POSITION+1, 2*MAX_TRANSACTION_POSITION);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRange(pos), std::runtime_error);

    pos = *rc::gen::inRange(-MAX_TRANSACTION_POSITION_TESTNET, -1);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRangeTestnet(pos), std::runtime_error);

    pos = *rc::gen::inRange(MAX_TRANSACTION_POSITION_TESTNET+1, 2*MAX_TRANSACTION_POSITION_TESTNET);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRangeTestnet(pos), std::runtime_error);
}

// check that we accept magic codes within the correct range
TEST(TxrefTest, accept_good_magic_code) {
    EXPECT_NO_THROW(checkMagicCodeRange(0));
    EXPECT_NO_THROW(checkMagicCodeRange(1));
    EXPECT_NO_THROW(checkMagicCodeRange(MAX_MAGIC_CODE));
}

RC_GTEST_PROP(TxrefTestRC, goodMagicCodesAreAccepted, ()
) {
    // generate valid magic codes
    auto code = *rc::gen::inRange(0, MAX_MAGIC_CODE);
    checkMagicCodeRange(code);
}

// check that we accept magic codes outside of the range
TEST(TxrefTest, reject_bad_magic_code) {
    EXPECT_THROW(checkMagicCodeRange(-1), std::runtime_error);
    EXPECT_THROW(checkMagicCodeRange(MAX_MAGIC_CODE + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badMagicCodesAreRejected, ()
) {
    // generate out of range magic codes
    auto code = *rc::gen::inRange(-MAX_MAGIC_CODE, -1);
    RC_ASSERT_THROWS_AS(checkMagicCodeRange(code), std::runtime_error);

    code = *rc::gen::inRange(MAX_MAGIC_CODE + 1, 2 * MAX_MAGIC_CODE);
    RC_ASSERT_THROWS_AS(checkMagicCodeRange(code), std::runtime_error);
}

// check that we correctly encode some sample txrefs
TEST(TxrefTest, txref_encode_mainnet) {
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0),
              "tx1-rqqq-qqqq-qmhu-qk");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x1FFF),
              "tx1-rqqq-qull-6v87-r7");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x1FFFFF, 0),
              "tx1-r7ll-lrqq-vq5e-gg");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x1FFFFF, 0x1FFF),
              "tx1-r7ll-llll-khym-tq");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205),
              "tx1-rjk0-u5ng-4jsf-mc");
}

// check that we correctly encode some sample txrefs for testnet
TEST(TxrefTest, txref_encode_testnet) {
    EXPECT_EQ(txrefEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0),
              "txtest1-xqqq-qqqq-qqkn-3gh9");
    EXPECT_EQ(txrefEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0x3FFFFFF, 0x3FFFF),
              "txtest1-x7ll-llll-llj9-t9dk");
    EXPECT_EQ(txrefEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 467883, 2355),
              "txtest1-xk63-uqvx-fqx8-xqr8");
}

// check that we can extract the block height from txrefs for both main and testnet
TEST(TxrefTest, extract_block_height) {
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

// check that we can extract the block height from extended txrefs for both main and testnet
TEST(TxrefTest, extract_extended_block_height) {
    std::string txref;
    int blockHeight;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqqquau7hl";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1rqqqqqqqyrq9mqh4w";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1r7lllrqqqqqmm5vjv";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0x1FFFFF);

    txref = "tx1rqqqqqqqll8temcac";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "txtest1xqqqqqqqqqqqqj7dvzy";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "txtest1xqqqqqulllyrqajp7n7";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "txtest1x7llllrqqqgxq0njv4n";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0x3FFFFFF);

    txref = "txtest1xjk0uq5ngq2qqjtrcwr";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

}

// check that we can extract the transaction position from txrefs for both main and testnet
TEST(TxrefTest, extract_transaction_position) {
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

// check that we can add missing standard HRPs if needed
TEST(TxrefTest, txref_add_hrps) {
    std::string txref;

    txref = "rqqqqqqqqmhuqk";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1rqqqqqqqqmhuqk");

    txref = "xk63uqvxfqx8xqr8";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest1xk63uqvxfqx8xqr8");

    txref = "rqqqqqqqqqquau7hl";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1rqqqqqqqqqquau7hl");

    txref = "xqqqqqqqqqyrqtc39q4";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest1xqqqqqqqqqyrqtc39q4");
}

// check that we correctly encode extended txrefs
TEST(TxrefTest, txref_extended_encode_mainnet) {

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0, 0),
              "tx1-rqqq-qqqq-qqqu-au7hl");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0, 100),
              "tx1-rqqq-qqqq-yrq9-mqh4w");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0, 0x1FFF),
              "tx1-rqqq-qqqq-ll8t-emcac");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x1FFF, 0),
              "tx1-rqqq-qull-qqq5-ktx95");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x1FFF, 100),
              "tx1-rqqq-qull-yrqd-sh089");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x1FFFFF, 0, 0),
              "tx1-r7ll-lrqq-qqqm-m5vjv");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x1FFFFF, 0, 200),
              "tx1-r7ll-lrqq-gxqq-h97k8");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x1FFFFF, 0x1FFF, 0),
              "tx1-r7ll-llll-qqqn-sr5q8");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x1FFFFF, 0x1FFF, 0x1FFF),
              "tx1-r7ll-llll-ll8y-5yj2q");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205, 0),
              "tx1-rjk0-u5ng-qqq8-lsnk3");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205, 10),
              "tx1-rjk0-u5ng-2qqn-tcg6h");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205, 0x1FFF),
              "tx1-rjk0-u5ng-ll8s-mh4uk");

    // These tests come from the examples in the BIP for the txref-ext
    // changes. Some may duplicate tests above, but they are all here in
    // one spot for reference.

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, 0xB, 0x1FFFFF, 0x1FFF, 0),
              "tx1-t7ll-llll-qqqt-433dq");

}

// check that we correctly encode extended txrefs
TEST(TxrefTest, txref_extended_encode_testnet) {

    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0, 0),
              "txtest1-xqqq-qqqq-qqqq-qj7dvzy");
    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0, 100),
              "txtest1-xqqq-qqqq-qqyr-qtc39q4");
    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0, 0x1FFF),
              "txtest1-xqqq-qqqq-qqll-89622gr");

    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0x3FFFF, 0),
              "txtest1-xqqq-qqul-llqq-qy5ah30");
    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0, 0x3FFFF, 100),
              "txtest1-xqqq-qqul-llyr-qajp7n7");

    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0x3FFFFFF, 0, 0),
              "txtest1-x7ll-llrq-qqqq-q5lr73c");
    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0x3FFFFFF, 0, 200),
              "txtest1-x7ll-llrq-qqgx-q0njv4n");

    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0x3FFFFFF, 0x3FFFF, 0),
              "txtest1-x7ll-llll-llqq-qz4n9zn");
    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 0x3FFFFFF, 0x3FFFF, 0x1FFF),
              "txtest1-x7ll-llll-llll-8435rg5");

    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 466793, 2205, 0),
              "txtest1-xjk0-uq5n-gqqq-qxltrz9");
    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 466793, 2205, 10),
              "txtest1-xjk0-uq5n-gq2q-qjtrcwr");
    EXPECT_EQ(txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 466793, 2205, 0x1FFF),
              "txtest1-xjk0-uq5n-gqll-83mv9gz");
}

RC_GTEST_PROP(TxrefTestRC, checkThatEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);

    auto txref = txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, height, pos);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefTestRC, checkThatEncodeAndDecodeTestnetProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT_TESTNET);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION_TESTNET);

    auto txref = txrefEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, height, pos);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefTestRC, checkThatExtendedEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    auto index = *rc::gen::inRange(0, MAX_UTXO_INDEX);

    auto txref = txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, height, pos, index);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.uxtoIndex == index);
}

RC_GTEST_PROP(TxrefTestRC, checkThatExtendedEncodeAndDecodeTestnetProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT_TESTNET);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION_TESTNET);
    auto index = *rc::gen::inRange(0, MAX_UTXO_INDEX);

    auto txref = txrefExtEncodeTestnet(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, height, pos, index);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.uxtoIndex == index);
}

