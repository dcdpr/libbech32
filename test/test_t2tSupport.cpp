#include <gtest/gtest.h>
#include <json/json.h>

#include "txid2txref.h"
#include "t2tSupport.h"
#include "t2tSupport.cpp"
#include "bitcoinRPCFacade.h"
#include "bitcoinRPCFacade.cpp"

// TODO, this test isn't very good since either we make it an integration test (with bitcoind)
// or we have to mock just about everything t2t::decodeTxref() and t2t::encodeTxref() does

class FakeBitcoinRPCFacade : public BitcoinRPCFacade {
public:
    // empty constructor for testing
    FakeBitcoinRPCFacade()
            : BitcoinRPCFacade() {
    }

    blockchaininfo_t getblockchaininfo() const override {
        blockchaininfo_t ret;
        ret.chain = "test";
        return ret;
    }

};

TEST(t2tSupport, test_decodeTxref) {

    t2t::Config config;
    t2t::Transaction transaction;
    FakeBitcoinRPCFacade btc;

    config.query = "tx1-rjk0-u5ng-4jsf-mc";

//    t2t::decodeTxref(btc, config, transaction);
}

TEST(t2tSupport, test_decodeTxrefext) {

    t2t::Config config;
    t2t::Transaction transaction;
    FakeBitcoinRPCFacade btc;

    config.query = "tx1-rjk0-u5ng-2qqn-tcg6h";

//    t2t::decodeTxref(btc, config, transaction);
}
