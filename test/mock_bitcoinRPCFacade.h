
#ifndef BTCR_DID_MOCK_BITCOINRPCFACADE_H
#define BTCR_DID_MOCK_BITCOINRPCFACADE_H

#include <gmock/gmock.h>
#include <bitcoinapi/types.h>
#include "../src/bitcoinRPCFacade.h"


class MockBitcoinRPCFacade : public BitcoinRPCFacade {
public:
    MOCK_CONST_METHOD2(getrawtransaction,
            getrawtransaction_t(const std::string& txid, int verbose));
    MOCK_CONST_METHOD1(getblock,
            blockinfo_t(const std::string& blockhash));
    MOCK_CONST_METHOD1(getblockhash,
            std::string(int blocknumber));
    MOCK_CONST_METHOD2(gettxout,
            utxoinfo_t(const std::string& txid, int n));
    MOCK_CONST_METHOD2(createrawtransaction,
            std::string(const std::vector<txout_t>& inputs, const std::map<std::string, double>& amounts));
    MOCK_CONST_METHOD2(createrawtransaction,
            std::string(const std::vector<txout_t>& inputs, const std::map<std::string, std::string>& amounts));
    MOCK_CONST_METHOD4(signrawtransaction,
            std::string(const std::string& rawTx, const std::vector<signrawtxin_t> & inputs, const std::vector<std::string>& privkeys, const std::string& sighashtype));
    MOCK_CONST_METHOD0(getblockchaininfo,
            blockchaininfo_t());
};



#endif //BTCR_DID_MOCK_BITCOINRPCFACADE_H
