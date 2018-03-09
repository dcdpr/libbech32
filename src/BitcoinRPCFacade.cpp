#include "BitcoinRPCFacade.h"

#include <bitcoinapi/bitcoinapi.h>

namespace {

    int MAINNET_PORT = 8332;
    int TESTNET_PORT = 18332;

    bool isConnectionGood(BitcoinAPI *b) {
        try {
            b->getinfo();
        }
        catch (BitcoinException &e)
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

