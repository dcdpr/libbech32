#include "bitcoinRPCFacade.h"

#include <bitcoinapi/bitcoinapi.h>

namespace {

    int MAINNET_PORT = 8332;
    int TESTNET_PORT = 18332;

    bool isConnectionGood(BitcoinAPI *b) {
        try {
            b->getblockcount();
        }
        catch (BitcoinException &)
        {
            return false;
        }
        return true;
    }
}

BitcoinRPCFacade::BitcoinRPCFacade(
        const std::string &user,
        const std::string &password,
        const std::string &host,
        int port) {

    std::stringstream ss;
    if(port != 0) {
        bitcoinAPI = new BitcoinAPI(user, password, host, port);
        if (isConnectionGood(bitcoinAPI))
            return;
        ss << "Error: Can't connect to " << host  << " on port " << port;
    }
    else {
        bitcoinAPI = new BitcoinAPI(user, password, host, MAINNET_PORT);
        if (isConnectionGood(bitcoinAPI))
            return;
        bitcoinAPI = new BitcoinAPI(user, password, host, TESTNET_PORT);
        if (isConnectionGood(bitcoinAPI))
            return;
        ss << "Error: Can't connect to " << host  << " on either port (" << MAINNET_PORT << "," << TESTNET_PORT << ")";
    }
    throw std::runtime_error(ss.str());
}

BitcoinRPCFacade::~BitcoinRPCFacade() = default;

getrawtransaction_t BitcoinRPCFacade::getrawtransaction(const std::string &txid, int verbose) const {
    return bitcoinAPI->getrawtransaction(txid, verbose);
}

blockinfo_t BitcoinRPCFacade::getblock(const std::string &blockhash) const {
    return bitcoinAPI->getblock(blockhash);
}

std::string BitcoinRPCFacade::getblockhash(int blocknumber) const {
    return bitcoinAPI->getblockhash(blocknumber);
}

blockchaininfo_t BitcoinRPCFacade::getblockchaininfo() const {
    std::string command = "getblockchaininfo";
    Value params, result;
    blockchaininfo_t ret;
    result = bitcoinAPI->sendcommand(command, params);

    ret.chain = result["chain"].asString();
    ret.blocks = result["blocks"].asInt();
    ret.headers = result["headers"].asInt();
    ret.bestblockhash = result["bestblockhash"].asString();
    ret.difficulty = result["difficulty"].asDouble();
    ret.mediantime = result["mediantime"].asInt();
    ret.verificationprogress = result["verificationprogress"].asDouble();
    ret.chainwork = result["chainwork"].asString();
    ret.pruned = result["pruned"].asBool();
    ret.pruneheight = result["pruneheight"].asInt();

    return ret;
}

utxoinfo_t BitcoinRPCFacade::gettxout(const std::string &txid, int n) const {
    return bitcoinAPI->gettxout(txid, n);
}

std::string BitcoinRPCFacade::createrawtransaction(
        const std::vector<txout_t> &inputs,
        const std::map<std::string, double> &amounts) const {
    return bitcoinAPI->createrawtransaction(inputs, amounts);
}

std::string BitcoinRPCFacade::createrawtransaction(
        const std::vector<txout_t> &inputs,
        const std::map<std::string, std::string> &amounts) const {
    return bitcoinAPI->createrawtransaction(inputs, amounts);
}

std::string
BitcoinRPCFacade::signrawtransaction(const std::string &rawTx, const std::vector<signrawtxin_t> & inputs,
                                     const std::vector<std::string> &privkeys, const std::string &sighashtype) const {
    signrawtransaction_t signedTx = bitcoinAPI->signrawtransaction(rawTx, inputs, privkeys, sighashtype);
    if(signedTx.complete) {
        return signedTx.hex;
    }
    return ""; // TODO throw?
}

std::string BitcoinRPCFacade::sendrawtransaction(const std::string &hexString, bool highFee) const {
    return bitcoinAPI->sendrawtransaction(hexString, highFee);
}


