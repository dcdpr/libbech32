#include <json/json.h>
#include "chainSoQuery.h"
#include "CurlWrapper.h"
#include "satoshis.h"

namespace {

    const std::string CHAIN_SO_URL = "https://chain.so/api/v2";

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
        throw std::runtime_error("transactionIndex too large");

    unspentData.txid = tx["txid"].asString();
    unspentData.address = address;
    unspentData.scriptPubKeyHex = tx["script_hex"].asString();
    unspentData.amountSatoshis = btc2satoshi(std::atof(tx["value"].asString().data()));
    unspentData.index = tx["output_no"].asInt();

    return unspentData;
}

