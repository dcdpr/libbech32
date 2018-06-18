#include <json/json.h>
#include <sstream>
#include "chainSoQuery.h"
#include "CurlWrapper.h"
#include "satoshis.h"

namespace {

    const char CHAIN_SO_URL[] = "https://chain.so/api/v2";
    const char GET_TX_UNSPENT[] = "get_tx_unspent";
    const char MAINNET_CODE[] = "BTC";
    const char TESTNET_CODE[] = "BTCTEST";

    std::string getUrl(const std::string &network, const std::string &address) {
        std::string networkCode = network == "main" ? MAINNET_CODE : TESTNET_CODE;
        std::string url = CHAIN_SO_URL;
        url += "/";
        url += GET_TX_UNSPENT;
        url += "/";
        url += networkCode;
        url += "/";
        url += address;
        return url;
    }
}


UnspentData ChainSoQuery::getUnspentOutputs(
        const std::string & address, int utxoIndex, const std::string & network) const {

    return populateFromJson(address, utxoIndex,
                            retrieveJsonData(getUrl(network, address)));
}

std::string ChainSoQuery::retrieveJsonData(const std::string &url) const {
    CurlWrapper curl;
    std::string jsonData = curl.download(url);
    return jsonData;
}

UnspentData ChainSoQuery::populateFromJson(
        const std::string & address, int utxoIndex, const std::string & jsonData) const {

    UnspentData unspentData = {};

    Json::Value obj;

    std::istringstream iss (jsonData);
    iss >> obj;

    Json::Value tx = obj["data"]["txs"][utxoIndex];
    if(tx.type() == Json::nullValue) {
        std::stringstream ss;
        ss << "UTXO not found for utxoIndex: " << utxoIndex;
        throw std::runtime_error(ss.str());
    }

    unspentData.txid = tx["txid"].asString();
    unspentData.address = address;
    unspentData.scriptPubKeyHex = tx["script_hex"].asString();
    unspentData.amountSatoshis = btc2satoshi(std::atof(tx["value"].asString().data()));
    unspentData.utxoIndex = tx["output_no"].asInt();

    return unspentData;
}

