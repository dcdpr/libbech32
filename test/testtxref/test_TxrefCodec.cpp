#include <gtest/gtest.h>
#pragma clang diagnostic push
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <rapidcheck/gtest.h>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#include "txrefCodec.cpp"

// check that we accept block heights within the correct range
TEST(TxrefTest, accept_good_block_heights) {
    EXPECT_NO_THROW(checkBlockHeightRange(0));
    EXPECT_NO_THROW(checkBlockHeightRange(1));
    EXPECT_NO_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT));
}

RC_GTEST_PROP(TxrefTestRC, goodBlockHeightsAreAccepted, ()
) {
    // generate valid block heights
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    checkBlockHeightRange(height);
}


// check that we reject block heights outside of the range
TEST(TxrefTest, reject_bad_block_heights) {
    EXPECT_THROW(checkBlockHeightRange(-1), std::runtime_error);
    EXPECT_THROW(checkBlockHeightRange(MAX_BLOCK_HEIGHT + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badBlockHeightsAreRejected, ()
) {
    // generate out of range block heights
    auto height = *rc::gen::inRange(-MAX_BLOCK_HEIGHT, -1);
    RC_ASSERT_THROWS_AS(checkBlockHeightRange(height), std::runtime_error);

    height = *rc::gen::inRange(MAX_BLOCK_HEIGHT+1, 2*MAX_BLOCK_HEIGHT);
    RC_ASSERT_THROWS_AS(checkBlockHeightRange(height), std::runtime_error);
}


// check that we accept transaction positions within the correct range
TEST(TxrefTest, accept_good_transaction_position) {
    EXPECT_NO_THROW(checkTransactionPositionRange(0));
    EXPECT_NO_THROW(checkTransactionPositionRange(1));
    EXPECT_NO_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION));
}

RC_GTEST_PROP(TxrefTestRC, goodTransactionPositionsAreAccepted, ()
) {
    // generate valid transaction positions
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    checkTransactionPositionRange(pos);
}


// check that we reject transaction positions outside of the range
TEST(TxrefTest, reject_bad_transaction_position) {
    EXPECT_THROW(checkTransactionPositionRange(-1), std::runtime_error);
    EXPECT_THROW(checkTransactionPositionRange(MAX_TRANSACTION_POSITION + 1), std::runtime_error);
}

RC_GTEST_PROP(TxrefTestRC, badTransactionPositionsAreRejected, ()
) {
    // generate out of range transaction positions
    auto pos = *rc::gen::inRange(-MAX_TRANSACTION_POSITION, -1);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRange(pos), std::runtime_error);

    pos = *rc::gen::inRange(MAX_TRANSACTION_POSITION+1, 2*MAX_TRANSACTION_POSITION);
    RC_ASSERT_THROWS_AS(checkTransactionPositionRange(pos), std::runtime_error);
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

// check that we can extract the magic code for a txref string
TEST(TxrefTest, extract_magicCode) {
    std::string txref;
    uint8_t magicCode;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqmhuqhp";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_MAIN);

    txref = "txtest1xjk0uqayzat0dz8";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_TEST);

}

// check that we can extract the version for a txref string
TEST(TxrefTest, extract_version) {
    std::string txref;
    uint8_t version;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqmhuqhp";
    bs = bech32::decode(txref);
    extractVersion(version, bs);
    EXPECT_EQ(version, 0); // regular txref

    txref = "tx1rpqqqqqqqqqqq2geahz";
    bs = bech32::decode(txref);
    extractVersion(version, bs);
    EXPECT_EQ(version, 1); // extended txref
}

// check that we can extract the block height from txrefs
TEST(TxrefTest, extract_block_height) {
    std::string txref;
    int blockHeight;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqmhuqhp";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1rqqqqqlll8xhjkg";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1r7llllqqqghqqr8";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1r7lllllll5xtjzw";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1rjk0uqayzsrwhqe";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

}

// check that we can extract the transaction position from txrefs
TEST(TxrefTest, extract_transaction_position) {
    std::string txref;
    int transactionPosition;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqmhuqhp";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1rqqqqqlll8xhjkg";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1r7llllqqqghqqr8";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1r7lllllll5xtjzw";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1rjk0uqayzsrwhqe";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);
}

// check that we can add missing standard HRPs if needed
TEST(TxrefTest, txref_add_hrps) {
    std::string txref;

    txref = "rqqqqqqqqmhuqhp";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1rqqqqqqqqmhuqhp");

    txref = "xjk0uqayzat0dz8";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest1xjk0uqayzat0dz8");

}

// check that we correctly encode some sample txrefs
TEST(TxrefTest, txref_encode_mainnet) {
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0),
              "tx1-rqqq-qqqq-qmhu-qhp");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x7FFF),
              "tx1-rqqq-qqll-l8xh-jkg");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0),
              "tx1-r7ll-llqq-qghq-qr8");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x7FFF),
              "tx1-r7ll-llll-l5xt-jzw");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205),
              "tx1-rjk0-uqay-zsrw-hqe");

    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 466793, 2205),
              "txtest1-xjk0-uqay-zat0-dz8");
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

// //////////////// Extended Txrefs /////////////////////

// check that we can extract the magic code for an extended txref string
TEST(TxrefTest, extract_extended_magicCode) {
    std::string txref;
    uint8_t magicCode;
    bech32::HrpAndDp bs;

    txref = "tx1rpqqqqqqqqqqq2geahz";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_MAIN);

    txref = "txtest1xpjk0uqayzu4xgrlpue";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_TEST);

}

// check that we can extract the extended version for an extended txref string
TEST(TxrefTest, extract_extended_version) {
    std::string txref;
    uint8_t extendedVersion;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqmhuqhp";
    bs = bech32::decode(txref);
    EXPECT_THROW(extractExtendedVersion(extendedVersion, bs), std::runtime_error);

    txref = "tx1rpqqqqqqqqqqq2geahz";
    bs = bech32::decode(txref);
    extractExtendedVersion(extendedVersion, bs);
    EXPECT_EQ(extendedVersion, 0);

    txref = "tx1rrqqqqqqqqqqqah3354";
    bs = bech32::decode(txref);
    extractExtendedVersion(extendedVersion, bs);
    EXPECT_EQ(extendedVersion, 1);

    txref = "tx1r9qqqqqqqqqqqdlf939";
    bs = bech32::decode(txref);
    extractExtendedVersion(extendedVersion, bs);
    EXPECT_EQ(extendedVersion, 2);

    txref = "tx1r4qqqqqqqqqqq33m7fe";
    bs = bech32::decode(txref);
    extractExtendedVersion(extendedVersion, bs);
    EXPECT_EQ(extendedVersion, 10);

    txref = "tx1rlpqqqqqqqqqqtcj6km";
    bs = bech32::decode(txref);
    extractExtendedVersion(extendedVersion, bs);
    EXPECT_EQ(extendedVersion, 31);

}

// check that we can extract the block height from extended txrefs
TEST(TxrefTest, extract_extended_block_height) {
    std::string txref;
    int blockHeight;
    bech32::HrpAndDp bs;

    txref = "tx1rpqqqqqqqqqqq2geahz";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1rp7llllqqqqqqpvhwkq";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1rpjk0uqayzu4x0w0kuq";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

    txref = "txtest1xpjk0uqayzu4xgrlpue";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

}

// check that we can extract the transaction position from extended txrefs
TEST(TxrefTest, extract_extended_transaction_position) {
    std::string txref;
    int transactionPosition;
    bech32::HrpAndDp bs;

    txref = "tx1rpqqqqqqqqqqq2geahz";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1rpqqqqqlllqqqshzqhw";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1rpjk0uqayzu4x0w0kuq";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);

    txref = "txtest1xpjk0uqayzu4xgrlpue";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);

}

// check that we can extract the txo index from extended txrefs
TEST(TxrefTest, extract_extended_txo_index) {
    std::string txref;
    int txoIndex;
    bech32::HrpAndDp bs;

    txref = "tx1rpqqqqqqqqqqq2geahz";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1rpqqqqqqqqpqqg2s2w6";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 1);

    txref = "tx1rp7llllqqqu4xs4zllp";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

    txref = "tx1rpjk0uqayzu4x0w0kuq";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

    txref = "txtest1xpjk0uqayzu4xgrlpue";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

}

// check that we can add missing standard HRPs if needed
TEST(TxrefTest, txref_add_hrps_extended) {
    std::string txref;

    txref = "rpjk0uqayzu4x0w0kuq";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1rpjk0uqayzu4x0w0kuq");

    txref = "xpjk0uqayzu4xgrlpue";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest1xpjk0uqayzu4xgrlpue");

}

// check that we correctly encode some sample extended txrefs
TEST(TxrefTest, txref_encode_extended_mainnet) {
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0, 0),
              "tx1-rpqq-qqqq-qqqq-q2geahz");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x7FFF, 0),
              "tx1-rpqq-qqql-llqq-qshzqhw");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0, 0),
              "tx1-rp7l-lllq-qqqq-qpvhwkq");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x7FFF, 0),
              "tx1-rp7l-llll-llqq-qmnvnkv");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0, 1),
              "tx1-rpqq-qqqq-qqpq-qg2s2w6");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x7FFF, 1),
              "tx1-rpqq-qqql-llpq-qj4thwk");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0, 1),
              "tx1-rp7l-lllq-qqpq-qrw7e0c");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x7FFF, 1),
              "tx1-rp7l-llll-llpq-qe39y05");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0, 0x1ABC),
              "tx1-rpqq-qqqq-qqu4-xm3vv7r");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x7FFF, 0x1ABC),
              "tx1-rpqq-qqql-llu4-xpwh370");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0, 0x1ABC),
              "tx1-rp7l-lllq-qqu4-xs4zllp");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x7FFF, 0x1ABC),
              "tx1-rp7l-llll-llu4-x22ezld");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205, 0x1ABC),
              "tx1-rpjk-0uqa-yzu4-x0w0kuq");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 466793, 2205, 0x1ABC),
              "txtest1-xpjk-0uqa-yzu4-xgrlpue");
}

RC_GTEST_PROP(TxrefTestRC, checkThatEncodeAndDecodeProduceSameExtendedParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    auto txoIndex = *rc::gen::inRange(0, MAX_TXO_INDEX);

    auto txref = txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, height, pos, txoIndex);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);
}
