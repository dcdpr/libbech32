#include <json/json.h>
#include "ChainSoQuery.h"
#include "CurlWrapper.h"

namespace {

    const std::string CHAIN_SO_URL = "https://chain.so/api/v2";

    const int SATOSHIS_PER_BTC = 100000000;

    int64_t JSONtoAmountInSatoshis(double value) {
        return (int64_t)(value * SATOSHIS_PER_BTC + (value < 0.0 ? -.5 : .5));
    }

}


UnspentData ChainSoQuery::getUnspentOutputs(
        const std::string & address, int transactionIndex, const std::string & network) const {

    std::string networkCode = network == "main" ? "BTC" : "BTCTEST";

    std::string url = CHAIN_SO_URL + "/get_tx_unspent/" + networkCode + "/" + address;

    std::string jsonData = retrieveJsonData(url);

    return populateFromJson(address, transactionIndex, jsonData);
}

std::string ChainSoQuery::retrieveJsonData(const std::string &url) const {
    CurlWrapper curl;
    std::string jsonData = curl.download(url);
    return jsonData;
}

UnspentData ChainSoQuery::populateFromJson(
        const std::string & address, int transactionIndex, const std::string & jsonData) const {

    UnspentData unspentData = {};

    Json::Reader reader;
    Json::Value obj;
    reader.parse(jsonData, obj, false);

    Json::Value tx = obj["data"]["txs"][transactionIndex];
    if(tx.type() == Json::nullValue)
        return unspentData;

    unspentData.txid = tx["txid"].asString();
    unspentData.address = address;
    unspentData.scriptPubKeyHex = tx["script_hex"].asString();
    unspentData.amountSatoshis = JSONtoAmountInSatoshis(std::atof(tx["value"].asString().data()));
    unspentData.index = tx["output_no"].asInt();

    return unspentData;
}

