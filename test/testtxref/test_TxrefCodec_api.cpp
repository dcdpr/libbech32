#include <gtest/gtest.h>
#pragma clang diagnostic push
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <rapidcheck/gtest.h>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

#include "txrefCodec.h"

// In this "API" test file, we should only be referring to symbols in the "txref" namespace.

//check that we correctly decode txrefs for both main and testnet
TEST(TxrefApiTest, txref_decode) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1-rqqq-qqqq-qmhu-qhp";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "tx1-rqqq-qqll-l8xh-jkg";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0x7FFF);

    txref = "tx1-r7ll-llqq-qghq-qr8";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0xFFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "tx1-r7ll-llll-l5xt-jzw";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0xFFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x7FFF);

    txref = "tx1-rjk0-uqay-zsrw-hqe";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);

    txref = "txtest1-xjk0-uqay-zat0-dz8";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);

}

// check that we can deal with weird txref formatting (invalid characters)
TEST(TxrefApiTest, txref_decode_weird_formatting) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1---rqqq-<qqqq>-q>m<hu-q#h#p--";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txref, "tx1-rqqq-qqqq-qmhu-qhp");

    txref = "tx1-rqqq qqqq qmhu qhp";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txref, "tx1-rqqq-qqqq-qmhu-qhp");

    txref = "tx1rqqq,qqqq.qmhu.qhp";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txref, "tx1-rqqq-qqqq-qmhu-qhp");

    txref = "tx@test1-xj$$k0-uq@@ay---zat0-d%$z 8";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txref, "txtest1-xjk0-uqay-zat0-dz8");

}


// check that we can deal with missing HRPs at the start of the txref
TEST(TxrefApiTest, txref_decode_no_HRPs) {
    std::string txref;
    txref::LocationData loc;

    txref = "rqqq-qqqq-qmhu-qhp";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txref, "tx1-rqqq-qqqq-qmhu-qhp");

    txref = "xjk0-uqay-zat0-dz8";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txref, "txtest1-xjk0-uqay-zat0-dz8");

}


// check that we correctly decode extended txrefs for both main and testnet
TEST(TxrefApiTest, txref_extended_decode) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1-rpqq-qqqq-qqqq-q2geahz";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "tx1-rp7l-lllq-qqqq-qpvhwkq";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0xFFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "tx1-rpjk-0uqa-yzu4-x0w0kuq";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0x1ABC);

    txref = "txtest1-xpjk-0uqa-yzu4-xgrlpue";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0x1ABC);

}

//check that we correctly decode extended txrefs even with missing HRPs and weird formatting
TEST(TxrefApiTest, txref_extended_decode_weird) {
    std::string txref;
    txref::LocationData loc;

    txref = "r p#q q$-q %qq^q- &qq* qq( -q)2g)ea  hz";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "rp 7 l - l l l q - q q q q - q p v h w k q";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0xFFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txoIndex, 0);

    txref = "r@#$pjk$%-0u$%^qa-%^&yzu^&*4-x&*(0w0&%$%$kuq";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0x1ABC);

    txref = "xpj..k...-0..uqa.-.y.z.u4-...xg..rl...pue....     ";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.txoIndex, 0x1ABC);
}

// check that we correctly encode txrefs in main and testnet
TEST(TxrefApiTest, txref_encode) {
    EXPECT_EQ(txref::encode(0, 0),
              "tx1-rqqq-qqqq-qmhu-qhp");
    EXPECT_EQ(txref::encode(0, 0x7FFF),
              "tx1-rqqq-qqll-l8xh-jkg");
    EXPECT_EQ(txref::encode(0xFFFFFF, 0),
              "tx1-r7ll-llqq-qghq-qr8");
    EXPECT_EQ(txref::encode(0xFFFFFF, 0x7FFF),
              "tx1-r7ll-llll-l5xt-jzw");
    EXPECT_EQ(txref::encode(466793, 2205),
              "tx1-rjk0-uqay-zsrw-hqe");
    EXPECT_EQ(txref::encodeTestnet(0, 0),
              "txtest1-xqqq-qqqq-qkla-64l");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF),
              "txtest1-x7ll-llll-lew2-gqs");
    EXPECT_EQ(txref::encodeTestnet(467883, 2355),
              "txtest1-xk63-uqnf-zasf-wgq");
}

// check that we correctly encode extended txrefs for main and testnet
TEST(TxrefApiTest, txref_extended_encode) {

    EXPECT_EQ(txref::encode(0, 0, 100),
              "tx1-rpqq-qqqq-qqyr-qnw954n");

    EXPECT_EQ(txref::encode(0, 0, 0x1FFF),
              "tx1-rpqq-qqqq-qqll-8av7ma9");

    EXPECT_EQ(txref::encode(0, 0x1FFF, 100),
              "tx1-rpqq-qqql-l8yr-qx4tctr");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 200),
              "tx1-rp7l-llrq-qqgx-qk4d6pv");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0x1FFF),
              "tx1-rp7l-llrl-l8ll-80x4z3s");

    EXPECT_EQ(txref::encode(466793, 2205, 10),
              "tx1-rpjk-0uqa-yz2q-q2rjue8");

    EXPECT_EQ(txref::encode(466793, 2205, 0x1FFF),
              "tx1-rpjk-0uqa-yzll-8fnaplx");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 100),
              "txtest1-xpqq-qqqq-qqyr-q5r4r42");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 0x1FFF),
              "txtest1-xpqq-qqqq-qqll-86pwvau");

    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 100),
              "txtest1-xpqq-qqql-llyr-qwuw74x");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 200),
              "txtest1-xp7l-lllq-qqgx-qadktjj");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0x1FFF),
              "txtest1-xp7l-llll-llll-8t6mzuj");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 10),
              "txtest1-xpjk-0uqa-yz2q-qdwzte7");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0x1FFF),
              "txtest1-xpjk-0uqa-yzll-8w7dkll");
}

// check that we return regular txref for txoIndex=0, unless forceExtended is true
TEST(TxrefApiTest, txref_extended_encode_force_zero) {

    EXPECT_EQ(txref::encode(0, 0, 0),
              "tx1-rqqq-qqqq-qmhu-qhp");
    EXPECT_EQ(txref::encode(0, 0, 0, true),
              "tx1-rpqq-qqqq-qqqq-q2geahz");

    EXPECT_EQ(txref::encode(0, 0x1FFF, 0),
              "tx1-rqqq-qqll-8vnm-xax");
    EXPECT_EQ(txref::encode(0, 0x1FFF, 0, true),
              "tx1-rpqq-qqql-l8qq-qlnh3fj");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0),
              "tx1-r7ll-lrqq-q32l-zcx");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0, true),
              "tx1-rp7l-llrq-qqqq-qdeug98");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0),
              "tx1-r7ll-lrll-8xwc-yjp");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0, true),
              "tx1-rp7l-llrl-l8qq-qczjymh");

    EXPECT_EQ(txref::encode(466793, 2205, 0),
              "tx1-rjk0-uqay-zsrw-hqe");
    EXPECT_EQ(txref::encode(466793, 2205, 0, true),
              "tx1-rpjk-0uqa-yzqq-q7h684p");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 0),
              "txtest1-xqqq-qqqq-qkla-64l");
    EXPECT_EQ(txref::encodeTestnet(0, 0, 0, true),
              "txtest1-xpqq-qqqq-qqqq-qd9f2hm");

    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 0),
              "txtest1-xqqq-qqll-l2wk-g5k");
    EXPECT_EQ(txref::encodeTestnet(0, 0x7FFF, 0, true),
              "txtest1-xpqq-qqql-llqq-qh6jhhh");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 0),
              "txtest1-x7ll-llqq-q9lp-6pe");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0, 0, true),
              "txtest1-xp7l-lllq-qqqq-qxp8eke");

    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0),
              "txtest1-x7ll-llll-lew2-gqs");
    EXPECT_EQ(txref::encodeTestnet(0xFFFFFF, 0x7FFF, 0, true),
              "txtest1-xp7l-llll-llqq-qu7uyk4");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0),
              "txtest1-xjk0-uqay-zat0-dz8");
    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0, true),
              "txtest1-xpjk-0uqa-yzqq-qe62s4c");
}

//TEST(TxrefApiTest, txref_extended_encode_BIP_examples) {
//
//    // These tests come from the examples in the BIP for the txref-ext
//    // changes. Some may duplicate tests above, but they are all here in
//    // one spot for reference.
//
//    EXPECT_EQ(txref::encode(0, 0, 0, true),               "tx1-rqqq-qqqq-qqqu-au7hl");
//    EXPECT_EQ(txref::encode(0, 0, 100),                   "tx1-rqqq-qqqq-yrq9-mqh4w");
//    EXPECT_EQ(txref::encode(0, 0, 0x1FFF),                "tx1-rqqq-qqqq-ll8t-emcac");
//    EXPECT_EQ(txref::encode(0, 0x1FFF, 0, true),          "tx1-rqqq-qull-qqq5-ktx95");
//    EXPECT_EQ(txref::encode(0, 0x1FFF, 100),              "tx1-rqqq-qull-yrqd-sh089");
//    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0, true),        "tx1-r7ll-lrqq-qqqm-m5vjv");
//    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0, true),   "tx1-r7ll-llll-qqqn-sr5q8");
//    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 100, true), "tx1-r7ll-llll-yrq2-klazk");
//}

//TEST(TxrefApiTest, txref_extended_decode_BIP_examples) {
//    std::string txref;
//    txref::LocationData loc;
//
//    // These tests come from the examples in the BIP for the txref-ext
//    // changes. Some may duplicate tests above, but they are all here in
//    // one spot for reference.
//
//    txref = "tx1:rjk0-u5ng-qqq8-lsnk3";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.txoIndex, 0);
//
//    txref = "tx1:rjk0-u5ng-dqqa-9wk8d";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.txoIndex, 13);
//
//
//    txref = "TX1RJK0U5NGQQQ8LSNK3";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.txoIndex, 0);
//
//    txref = "TX1RJK0U5NGDQQA9WK8D";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.txoIndex, 13);
//
//
////    txref = "TX1R1JK0--U5bNGQQQ8----LSNK3";
////    loc = txref::decode(txref);
////    EXPECT_EQ(loc.blockHeight, 0x71F69);
////    EXPECT_EQ(loc.transactionPosition, 0x89D);
////    EXPECT_EQ(loc.txoIndex, 0);
////
////    txref = "TX1R1JK0--U5bNGDQQa----9WK8D"; // fails because of extra '1' and mixed case? why is it listed in BIP?
////    loc = txref::decode(txref);
////    EXPECT_EQ(loc.blockHeight, 0x71F69);
////    EXPECT_EQ(loc.transactionPosition, 0x89D);
////    EXPECT_EQ(loc.txoIndex, 13);
//
//    txref = "tx1 rjk0 u5ng qqq8lsnk3";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.txoIndex, 0);
//
//    txref = "tx1 rjk0 u5ng dqqa9wk8d";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.txoIndex, 13);
//
//    txref = "tx1!rjk0\\u5ng*qqq8^^lsnk3";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.txoIndex, 0);
//
//    txref = "tx1!rjk0\\u5ng*dqqa^^9wk8d";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.txoIndex, 13);
//
//}

RC_GTEST_PROP(TxrefApiTestRC, checkThatEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION

    auto txref = txref::encode(height, pos);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatEncodeAndDecodeTestnetProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION

    auto txref = txref::encodeTestnet(height, pos);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatExtendedEncodeAndDecodeProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION
    auto index = *rc::gen::inRange(0, 0x7FFF); // MAX_TXO_INDEX

    auto txref = txref::encode(height, pos, index);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == index);
}

RC_GTEST_PROP(TxrefApiTestRC, checkThatExtendedEncodeAndDecodeTestnetProduceSameParameters, ()
) {
    auto height = *rc::gen::inRange(0, 0xFFFFFF); // MAX_BLOCK_HEIGHT_TESTNET
    auto pos = *rc::gen::inRange(0, 0x7FFF); // MAX_TRANSACTION_POSITION_TESTNET
    auto index = *rc::gen::inRange(0, 0x7FFF); // MAX_TXO_INDEX

    auto txref = txref::encodeTestnet(height, pos, index);
    auto loc = txref::decode(txref);

    RC_ASSERT(loc.blockHeight == height);
    RC_ASSERT(loc.transactionPosition == pos);
    RC_ASSERT(loc.txoIndex == index);
}

