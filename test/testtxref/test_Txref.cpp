#include <gtest/gtest.h>
#pragma clang diagnostic push
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <rapidcheck/gtest.h>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#include "txref.cpp"

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

// check that we reject magic codes outside of the range
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

TEST(TxrefTest, addDashes_inputStringTooShort) {
    EXPECT_THROW(addGroupSeparators("", 0, 1), std::runtime_error);
    EXPECT_THROW(addGroupSeparators("0", 0, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_HRPLongerThanInput) {

    // hrplen is zero, then the "rest" of the input is of length two, so one hyphen should be inserted
    auto result = addGroupSeparators("00", 0, 1);
    EXPECT_EQ(result, "0-0");

    // hrplen is one, then the "rest" of the input is of length one, so zero hyphens should be inserted
    result = addGroupSeparators("00", 1, 1);
    EXPECT_EQ(result, "00");

    // hrplen is two, then the "rest" of the input is of length zero, so zero hyphens should be inserted
    result = addGroupSeparators("00", 2, 1);
    EXPECT_EQ(result, "00");

    // hrplen is three, then the "rest" of the input is of length -1, so exception is thrown
    EXPECT_THROW(addGroupSeparators("00", 3, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_HRPTooLong) {
    EXPECT_THROW(addGroupSeparators("00", bech32::limits::MAX_HRP_LENGTH+1, 1), std::runtime_error);
}

TEST(TxrefTest, addDashes_separatorOffsetTooSmall) {
    EXPECT_THROW(addGroupSeparators("00", 0, -1), std::runtime_error);
    EXPECT_THROW(addGroupSeparators("00", 0, 0), std::runtime_error);
}

TEST(TxrefTest, addDashes_separatorOffsetTooLarge) {
    // if separatorOffset is greater than input string length, output should be the same
    auto result = addGroupSeparators("00", 0, 2);
    EXPECT_EQ(result, "00");
    result = addGroupSeparators("00", 0, 10);
    EXPECT_EQ(result, "00");
}

TEST(TxrefTest, addDashes_everyOtherCharacter) {
    auto result = addGroupSeparators("00", 0, 1);
    EXPECT_EQ(result, "0-0");

    result = addGroupSeparators("000", 0, 1);
    EXPECT_EQ(result, "0-0-0");

    result = addGroupSeparators("0000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0");

    result = addGroupSeparators("00000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0-0");
}

TEST(TxrefTest, addDashes_everyFewCharacters) {
    auto result = addGroupSeparators("0000000", 0, 1);
    EXPECT_EQ(result, "0-0-0-0-0-0-0");

    result = addGroupSeparators("0000000", 0, 2);
    EXPECT_EQ(result, "00-00-00-0");

    result = addGroupSeparators("0000000", 0, 3);
    EXPECT_EQ(result, "000-000-0");

    result = addGroupSeparators("0000000", 0, 4);
    EXPECT_EQ(result, "0000-000");
}

TEST(TxrefTest, addDashes_withHRPs) {
    auto result = addGroupSeparators("A0000000", 1, 1);
    EXPECT_EQ(result, "A0-0-0-0-0-0-0");

    result = addGroupSeparators("AB0000000", 2, 2);
    EXPECT_EQ(result, "AB00-00-00-0");

    result = addGroupSeparators("ABCD0000000", 4, 4);
    EXPECT_EQ(result, "ABCD0000-000");

}

TEST(TxrefTest, prettyPrint) {
    std::string hrp = txref::BECH32_HRP_MAIN;
    std::string plain = "tx1rqqqqqqqqmhuqhp";
    std::string pretty = prettyPrint(plain, hrp.length());
    EXPECT_EQ(pretty, "tx1:rqqq-qqqq-qmhu-qhp");
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
    EXPECT_EQ(version, 0);

    txref = "txtest1xjk0uqayzat0dz8";
    bs = bech32::decode(txref);
    extractVersion(version, bs);
    EXPECT_EQ(version, 0);
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

    txref = "txtest1xjk0uqayzat0dz8";
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

    txref = "txtest1xjk0uqayzat0dz8";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);
}

// check that extracting the txo index from txrefs always returns 0
TEST(TxrefTest, extract_txo_position) {
    std::string txref;
    int txoIndex;
    bech32::HrpAndDp bs;

    txref = "tx1rqqqqqqqqmhuqhp";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1rqqqqqlll8xhjkg";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1r7llllqqqghqqr8";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1r7lllllll5xtjzw";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1rjk0uqayzsrwhqe";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "txtest1xjk0uqayzat0dz8";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);
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
              "tx1:rqqq-qqqq-qmhu-qhp");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x7FFF),
              "tx1:rqqq-qqll-l8xh-jkg");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0),
              "tx1:r7ll-llqq-qghq-qr8");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x7FFF),
              "tx1:r7ll-llll-l5xt-jzw");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 466793, 2205),
              "tx1:rjk0-uqay-zsrw-hqe");

    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST, 466793, 2205),
              "txtest1:xjk0-uqay-zat0-dz8");
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

// check that we correctly encode some sample txrefs from BIP-136. These may duplicate
// some tests above, but many of the examples in the BIP are present here for reference.
TEST(TxrefTest, txref_encode_mainnet_bip_examples) {
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0),
              "tx1:rqqq-qqqq-qmhu-qhp");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0, 0x7FFF),
              "tx1:rqqq-qqll-l8xh-jkg");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0),
              "tx1:r7ll-llqq-qghq-qr8");
    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0xFFFFFF, 0x7FFF),
              "tx1:r7ll-llll-l5xt-jzw");

    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN, 0x71F69, 0x89D),
              "tx1:rjk0-uqay-zsrw-hqe");

    EXPECT_EQ(txrefEncode(txref::BECH32_HRP_MAIN, 0xB, 0xFFFFFF, 0x7FFF),
              "tx1:t7ll-llll-ldup-3hh");

}
//TODO what about the decode examples?

// //////////////// Extended Txrefs /////////////////////

// check for magic codes that support extended txrefs
TEST(TxrefTest, accept_good_magic_code_for_extended) {
    EXPECT_NO_THROW(checkExtendedMagicCode(txref::MAGIC_BTC_MAIN_EXTENDED));
    EXPECT_NO_THROW(checkExtendedMagicCode(txref::MAGIC_BTC_TEST_EXTENDED));

    EXPECT_THROW(checkExtendedMagicCode(txref::MAGIC_BTC_MAIN), std::runtime_error);
    EXPECT_THROW(checkExtendedMagicCode(txref::MAGIC_BTC_TEST), std::runtime_error);
}

// check that we can extract the magic code for an extended txref string
TEST(TxrefTest, extract_extended_magicCode) {
    std::string txref;
    uint8_t magicCode;
    bech32::HrpAndDp bs;

    txref = "tx1yjk0uqayzu4xnk6upc";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_MAIN_EXTENDED);

    txref = "txtest18jk0uqayzu4xaw4hzl";
    bs = bech32::decode(txref);
    extractMagicCode(magicCode, bs);
    EXPECT_EQ(magicCode, txref::MAGIC_BTC_TEST_EXTENDED);

}

// check that we can extract the block height from extended txrefs
TEST(TxrefTest, extract_extended_block_height) {
    std::string txref;
    int blockHeight;
    bech32::HrpAndDp bs;

    txref = "tx1yqqqqqqqqqqqksvh26";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0);

    txref = "tx1y7llllqqqqqqa5zytc";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 0xFFFFFF);

    txref = "tx1yjk0uqayzu4xnk6upc";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

    txref = "txtest18jk0uqayzu4xaw4hzl";
    bs = bech32::decode(txref);
    extractBlockHeight(blockHeight, bs);
    EXPECT_EQ(blockHeight, 466793);

}

// check that we can extract the transaction position from extended txrefs
TEST(TxrefTest, extract_extended_transaction_position) {
    std::string txref;
    int transactionPosition;
    bech32::HrpAndDp bs;

    txref = "tx1yqqqqqqqqqqqksvh26";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0);

    txref = "tx1yqqqqqlllqqqv0h22k";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 0x7FFF);

    txref = "tx1yjk0uqayzu4xnk6upc";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);

    txref = "txtest18jk0uqayzu4xaw4hzl";
    bs = bech32::decode(txref);
    extractTransactionPosition(transactionPosition, bs);
    EXPECT_EQ(transactionPosition, 2205);

}

// check that we can extract the txo index from extended txrefs
TEST(TxrefTest, extract_extended_txo_index) {
    std::string txref;
    int txoIndex;
    bech32::HrpAndDp bs;

    txref = "tx1yqqqqqqqqqqqksvh26";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0);

    txref = "tx1yqqqqqqqqpqq5j9qnz";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 1);

    txref = "tx1yqqqqqqqqu4x8fexrm";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

    txref = "tx1yjk0uqayzu4xnk6upc";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

    txref = "txtest18jk0uqayzu4xaw4hzl";
    bs = bech32::decode(txref);
    extractTxoIndex(txoIndex, bs);
    EXPECT_EQ(txoIndex, 0x1ABC);

}

// check that we can add missing standard HRPs if needed
TEST(TxrefTest, txref_add_hrps_extended) {
    std::string txref;

    txref = "yjk0uqayzu4xnk6upc";
    EXPECT_EQ(addHrpIfNeeded(txref), "tx1yjk0uqayzu4xnk6upc");

    txref = "8jk0uqayzu4xaw4hzl";
    EXPECT_EQ(addHrpIfNeeded(txref), "txtest18jk0uqayzu4xaw4hzl");

}

// check that we correctly encode some sample extended txrefs
TEST(TxrefTest, txref_encode_extended_mainnet) {
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 0),
              "tx1:yqqq-qqqq-qqqq-ksvh-26");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0x7FFF, 0),
              "tx1:yqqq-qqll-lqqq-v0h2-2k");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0, 0),
              "tx1:y7ll-llqq-qqqq-a5zy-tc");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 0),
              "tx1:y7ll-llll-lqqq-8tee-t5");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 1),
              "tx1:yqqq-qqqq-qpqq-5j9q-nz");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0x7FFF, 1),
              "tx1:yqqq-qqll-lpqq-wd7a-nw");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0, 1),
              "tx1:y7ll-llqq-qpqq-lktn-jq");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 1),
              "tx1:y7ll-llll-lpqq-9fsw-jv");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0, 0x1ABC),
              "tx1:yqqq-qqqq-qu4x-8fex-rm");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0, 0x7FFF, 0x1ABC),
              "tx1:yqqq-qqll-lu4x-akzm-rh");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0, 0x1ABC),
              "tx1:y7ll-llqq-qu4x-vdh4-ze");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 0xFFFFFF, 0x7FFF, 0x1ABC),
              "tx1:y7ll-llll-lu4x-kjvg-z4");

    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, 466793, 2205, 0x1ABC),
              "tx1:yjk0-uqay-zu4x-nk6u-pc");
    EXPECT_EQ(txrefExtEncode(txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST_EXTENDED, 466793, 2205, 0x1ABC),
              "txtest1:8jk0-uqay-zu4x-aw4h-zl");
}

RC_GTEST_PROP(TxrefTestRC, checkThatEncodeAndDecodeProduceSameExtendedParameters, ()
) {
    auto height = *rc::gen::inRange(0, MAX_BLOCK_HEIGHT);
    auto pos = *rc::gen::inRange(0, MAX_TRANSACTION_POSITION);
    auto txoIndex = *rc::gen::inRange(0, MAX_TXO_INDEX);

    auto txref = txrefExtEncode(txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN_EXTENDED, height, pos, txoIndex);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == txoIndex);
}
