#include <gtest/gtest.h>

#include "txref.h"

// In this "API" test file, we should only be referring to symbols in the "txref" namespace.

//check that we correctly decode txrefs for both main and testnet
TEST(TxrefApiTest, txref_decode) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1-rqqq-qqqq-qmhu-qk";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "tx1-rqqq-qull-6v87-r7";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0x1FFF);

    txref = "tx1-r7ll-lrqq-vq5e-gg";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0x1FFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "tx1-r7ll-llll-khym-tq";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0x1FFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x1FFF);

    txref = "tx1-rjk0-u5ng-4jsf-mc";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);


    txref = "txtest1-xqqq-qqqq-qqkn-3gh9";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);

    txref = "txtest1-x7ll-llll-llj9-t9dk";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0x3FFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x3FFFF);

    txref = "txtest1-xk63-uqvx-fqx8-xqr8";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 467883);
    EXPECT_EQ(loc.transactionPosition, 2355);

}

// check that we can deal with weird txref formatting (invalid characters)
TEST(TxrefApiTest, txref_decode_weird_formatting) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1---rqqq-<qqqq>-q>m<hu-q##k--";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txref, "tx1-rqqq-qqqq-qmhu-qk");

    txref = "tx1-rqqq qqqq qmhu qk";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txref, "tx1-rqqq-qqqq-qmhu-qk");

    txref = "tx1rqqq,qqqq.qmhu.qk";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txref, "tx1-rqqq-qqqq-qmhu-qk");

    txref = "tx@test1-xk$$63-uq@@vx---fqx8-xq%$r8";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 467883);
    EXPECT_EQ(loc.transactionPosition, 2355);
    EXPECT_EQ(loc.txref, "txtest1-xk63-uqvx-fqx8-xqr8");

}


// check that we can deal with missing HRPs at the start of the txref
TEST(TxrefApiTest, txref_decode_no_HRPs) {
    std::string txref;
    txref::LocationData loc;

    txref = "rqqq-qqqq-qmhu-qk";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.txref, "tx1-rqqq-qqqq-qmhu-qk");

    txref = "xk63-uqvx-fqx8-xqr8";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 467883);
    EXPECT_EQ(loc.transactionPosition, 2355);
    EXPECT_EQ(loc.txref, "txtest1-xk63-uqvx-fqx8-xqr8");

}


//check that we correctly decode extended txrefs for both main and testnet
TEST(TxrefApiTest, txref_extended_decode) {
    std::string txref;
    txref::LocationData loc;

    txref = "tx1-rqqq-qqqq-qqqu-au7hl";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "tx1-rqqq-qqqq-yrq9-mqh4w";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 100);

    txref = "tx1-r7ll-lrqq-qqqm-m5vjv";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0x1FFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "tx1-rqqq-qqqq-ll8t-emcac";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 0x1FFF);

    txref = "tx1-r7ll-llll-ll8y-5yj2q";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0x1FFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x1FFF);
    EXPECT_EQ(loc.uxtoIndex, 0x1FFF);


    txref = "txtest1-xqqq-qqqq-qqqq-qj7dvzy";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "txtest1-xqqq-qqul-llyr-qajp7n7";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0x3FFFF);
    EXPECT_EQ(loc.uxtoIndex, 100);

    txref = "txtest1-x7ll-llrq-qqgx-q0njv4n";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0x3FFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 200);

    txref = "txtest1-xjk0-uq5n-gq2q-qjtrcwr";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.uxtoIndex, 10);

    txref = "txtest1-x7ll-llll-llll-8435rg5";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0x3FFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x3FFFF);
    EXPECT_EQ(loc.uxtoIndex, 0x1FFF);
}

//check that we correctly decode extended txrefs even with missing HRPs and weird formatting
TEST(TxrefApiTest, txref_extended_decode_weird) {
    std::string txref;
    txref::LocationData loc;

    txref = "rqqq-qqqq-qqqu-au7hl";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "rq##qq-q#qqq---yrq9-m-q-h-4-w-";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 100);

    txref = "tx1-r7$$$$ll-l%%rqq-q  qqm m5vj v";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0x1FFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "tx1-rqq  q-q   qqq-l@@@@l8t-e(mcac)";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 0x1FFF);

    txref = "r7ll-llll-l                   l8y-5yj2q";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "tx");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_MAIN);
    EXPECT_EQ(loc.blockHeight, 0x1FFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x1FFF);
    EXPECT_EQ(loc.uxtoIndex, 0x1FFF);


    txref = "txtest1-xqqq-qqqq-qqqq-q!j!7!d!v!z!y";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "xqqq-qqul-llyr-qaj    p7n7";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0);
    EXPECT_EQ(loc.transactionPosition, 0x3FFFF);
    EXPECT_EQ(loc.uxtoIndex, 100);

    txref = "x7ll-llrq-q   qgx-q   0njv4n";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0x3FFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0);
    EXPECT_EQ(loc.uxtoIndex, 200);

    txref = "txtest1-xj!@k0-uq5n-g!@q2q-qjtr!@  cwr";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 466793);
    EXPECT_EQ(loc.transactionPosition, 2205);
    EXPECT_EQ(loc.uxtoIndex, 10);

    txref = "-x7ll-llll-llll-843(((()))))   5rg5";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.hrp, "txtest");
    EXPECT_EQ(loc.magicCode, txref::MAGIC_BTC_TEST);
    EXPECT_EQ(loc.blockHeight, 0x3FFFFFF);
    EXPECT_EQ(loc.transactionPosition, 0x3FFFF);
    EXPECT_EQ(loc.uxtoIndex, 0x1FFF);
}

// check that we correctly encode txrefs in main and testnet
TEST(TxrefApiTest, txref_encode) {
    EXPECT_EQ(txref::encode(0, 0),
              "tx1-rqqq-qqqq-qmhu-qk");
    EXPECT_EQ(txref::encode(0, 0x1FFF),
              "tx1-rqqq-qull-6v87-r7");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0),
              "tx1-r7ll-lrqq-vq5e-gg");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF),
              "tx1-r7ll-llll-khym-tq");
    EXPECT_EQ(txref::encode(466793, 2205),
              "tx1-rjk0-u5ng-4jsf-mc");
    EXPECT_EQ(txref::encodeTestnet(0, 0),
              "txtest1-xqqq-qqqq-qqkn-3gh9");
    EXPECT_EQ(txref::encodeTestnet(0x3FFFFFF, 0x3FFFF),
              "txtest1-x7ll-llll-llj9-t9dk");
    EXPECT_EQ(txref::encodeTestnet(467883, 2355),
              "txtest1-xk63-uqvx-fqx8-xqr8");
}

// check that we correctly encode extended txrefs for main and testnet
TEST(TxrefApiTest, txref_extended_encode) {

    EXPECT_EQ(txref::encode(0, 0, 100),
              "tx1-rqqq-qqqq-yrq9-mqh4w");

    EXPECT_EQ(txref::encode(0, 0, 0x1FFF),
              "tx1-rqqq-qqqq-ll8t-emcac");

    EXPECT_EQ(txref::encode(0, 0x1FFF, 100),
              "tx1-rqqq-qull-yrqd-sh089");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 200),
              "tx1-r7ll-lrqq-gxqq-h97k8");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0x1FFF),
              "tx1-r7ll-llll-ll8y-5yj2q");

    EXPECT_EQ(txref::encode(466793, 2205, 10),
              "tx1-rjk0-u5ng-2qqn-tcg6h");

    EXPECT_EQ(txref::encode(466793, 2205, 0x1FFF),
              "tx1-rjk0-u5ng-ll8s-mh4uk");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 100),
              "txtest1-xqqq-qqqq-qqyr-qtc39q4");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 0x1FFF),
              "txtest1-xqqq-qqqq-qqll-89622gr");

    EXPECT_EQ(txref::encodeTestnet(0, 0x3FFFF, 100),
              "txtest1-xqqq-qqul-llyr-qajp7n7");

    EXPECT_EQ(txref::encodeTestnet(0x3FFFFFF, 0, 200),
              "txtest1-x7ll-llrq-qqgx-q0njv4n");

    EXPECT_EQ(txref::encodeTestnet(0x3FFFFFF, 0x3FFFF, 0x1FFF),
              "txtest1-x7ll-llll-llll-8435rg5");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 10),
              "txtest1-xjk0-uq5n-gq2q-qjtrcwr");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0x1FFF),
              "txtest1-xjk0-uq5n-gqll-83mv9gz");
}

// check that we return regular txref for uxtoIndex=0, unless forceExtended is true
TEST(TxrefApiTest, txref_extended_encode_force_zero) {

    EXPECT_EQ(txref::encode(0, 0, 0),
              "tx1-rqqq-qqqq-qmhu-qk");
    EXPECT_EQ(txref::encode(0, 0, 0, true),
              "tx1-rqqq-qqqq-qqqu-au7hl");

    EXPECT_EQ(txref::encode(0, 0x1FFF, 0),
              "tx1-rqqq-qull-6v87-r7");
    EXPECT_EQ(txref::encode(0, 0x1FFF, 0, true),
              "tx1-rqqq-qull-qqq5-ktx95");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0),
              "tx1-r7ll-lrqq-vq5e-gg");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0, true),
              "tx1-r7ll-lrqq-qqqm-m5vjv");

    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0),
              "tx1-r7ll-llll-khym-tq");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0, true),
              "tx1-r7ll-llll-qqqn-sr5q8");

    EXPECT_EQ(txref::encode(466793, 2205, 0),
              "tx1-rjk0-u5ng-4jsf-mc");
    EXPECT_EQ(txref::encode(466793, 2205, 0, true),
              "tx1-rjk0-u5ng-qqq8-lsnk3");

    EXPECT_EQ(txref::encodeTestnet(0, 0, 0),
              "txtest1-xqqq-qqqq-qqkn-3gh9");
    EXPECT_EQ(txref::encodeTestnet(0, 0, 0, true),
              "txtest1-xqqq-qqqq-qqqq-qj7dvzy");

    EXPECT_EQ(txref::encodeTestnet(0, 0x3FFFF, 0),
              "txtest1-xqqq-qqul-llnl-9cdd");
    EXPECT_EQ(txref::encodeTestnet(0, 0x3FFFF, 0, true),
              "txtest1-xqqq-qqul-llqq-qy5ah30");

    EXPECT_EQ(txref::encodeTestnet(0x3FFFFFF, 0, 0),
              "txtest1-x7ll-llrq-qqhf-l4h7");
    EXPECT_EQ(txref::encodeTestnet(0x3FFFFFF, 0, 0, true),
              "txtest1-x7ll-llrq-qqqq-q5lr73c");

    EXPECT_EQ(txref::encodeTestnet(0x3FFFFFF, 0x3FFFF, 0),
              "txtest1-x7ll-llll-llj9-t9dk");
    EXPECT_EQ(txref::encodeTestnet(0x3FFFFFF, 0x3FFFF, 0, true),
              "txtest1-x7ll-llll-llqq-qz4n9zn");

    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0),
              "txtest1-xjk0-uq5n-gqum-rdcp");
    EXPECT_EQ(txref::encodeTestnet(466793, 2205, 0, true),
              "txtest1-xjk0-uq5n-gqqq-qxltrz9");
}

TEST(TxrefApiTest, txref_extended_encode_BIP_examples) {

    // These tests come from the examples in the BIP for the txref-ext
    // changes. Some may duplicate tests above, but they are all here in
    // one spot for reference.

    EXPECT_EQ(txref::encode(0, 0, 0, true),               "tx1-rqqq-qqqq-qqqu-au7hl");
    EXPECT_EQ(txref::encode(0, 0, 100),                   "tx1-rqqq-qqqq-yrq9-mqh4w");
    EXPECT_EQ(txref::encode(0, 0, 0x1FFF),                "tx1-rqqq-qqqq-ll8t-emcac");
    EXPECT_EQ(txref::encode(0, 0x1FFF, 0, true),          "tx1-rqqq-qull-qqq5-ktx95");
    EXPECT_EQ(txref::encode(0, 0x1FFF, 100),              "tx1-rqqq-qull-yrqd-sh089");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0, 0, true),        "tx1-r7ll-lrqq-qqqm-m5vjv");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 0, true),   "tx1-r7ll-llll-qqqn-sr5q8");
    EXPECT_EQ(txref::encode(0x1FFFFF, 0x1FFF, 100, true), "tx1-r7ll-llll-yrq2-klazk");
}

TEST(TxrefApiTest, txref_extended_decode_BIP_examples) {
    std::string txref;
    txref::LocationData loc;

    // These tests come from the examples in the BIP for the txref-ext
    // changes. Some may duplicate tests above, but they are all here in
    // one spot for reference.

    txref = "tx1:rjk0-u5ng-qqq8-lsnk3";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.blockHeight, 0x71F69);
    EXPECT_EQ(loc.transactionPosition, 0x89D);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "tx1:rjk0-u5ng-dqqa-9wk8d";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.blockHeight, 0x71F69);
    EXPECT_EQ(loc.transactionPosition, 0x89D);
    EXPECT_EQ(loc.uxtoIndex, 13);


    txref = "TX1RJK0U5NGQQQ8LSNK3";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.blockHeight, 0x71F69);
    EXPECT_EQ(loc.transactionPosition, 0x89D);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "TX1RJK0U5NGDQQA9WK8D";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.blockHeight, 0x71F69);
    EXPECT_EQ(loc.transactionPosition, 0x89D);
    EXPECT_EQ(loc.uxtoIndex, 13);


//    txref = "TX1R1JK0--U5bNGQQQ8----LSNK3";
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.uxtoIndex, 0);
//
//    txref = "TX1R1JK0--U5bNGDQQa----9WK8D"; // fails because of extra '1' and mixed case? why is it listed in BIP?
//    loc = txref::decode(txref);
//    EXPECT_EQ(loc.blockHeight, 0x71F69);
//    EXPECT_EQ(loc.transactionPosition, 0x89D);
//    EXPECT_EQ(loc.uxtoIndex, 13);

    txref = "tx1 rjk0 u5ng qqq8lsnk3";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.blockHeight, 0x71F69);
    EXPECT_EQ(loc.transactionPosition, 0x89D);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "tx1 rjk0 u5ng dqqa9wk8d";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.blockHeight, 0x71F69);
    EXPECT_EQ(loc.transactionPosition, 0x89D);
    EXPECT_EQ(loc.uxtoIndex, 13);

    txref = "tx1!rjk0\\u5ng*qqq8^^lsnk3";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.blockHeight, 0x71F69);
    EXPECT_EQ(loc.transactionPosition, 0x89D);
    EXPECT_EQ(loc.uxtoIndex, 0);

    txref = "tx1!rjk0\\u5ng*dqqa^^9wk8d";
    loc = txref::decode(txref);
    EXPECT_EQ(loc.blockHeight, 0x71F69);
    EXPECT_EQ(loc.transactionPosition, 0x89D);
    EXPECT_EQ(loc.uxtoIndex, 13);


}
