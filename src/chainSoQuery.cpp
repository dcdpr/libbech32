#include <unistd.h>
#include <sstream>
#include <utility>
#include <set>
#include <iostream>
#include <cassert>
#include <algorithm>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <json/json.h>
#pragma clang diagnostic pop

#include "chainSoQuery.h"
#include "curlWrapper.h"
#include "satoshis.h"

namespace {

    const char CHAIN_SO_URL[] = "https://chain.so/api/v2";
    const char MAINNET_CODE[] = "BTC";
    const char TESTNET_CODE[] = "BTCTEST";

    const char GET_TX_UNSPENT[] = "get_tx_unspent";
    const char IS_TX_SPENT[] = "is_tx_spent";
    const char GET_TX[] = "get_tx";

    /**
     * Construct a "get_tx_unspent" URL
     *
     * @param network Which bitcoin network ('main' or 'test')
     * @param address The bitcoin address to look up
     * @return The url
     */
    std::string getTxUnspentUrl(const std::string &network, const std::string &address) {
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

    /**
     * Construct an "is_tx_spent" URL
     *
     * @param network Which bitcoin network ('main' or 'test')
     * @param txid The bitcoin transaction id
     * @param txoIndex The index of the TXO
     * @return The url
     */
    std::string isTxSpentUrl(const std::string &network, const std::string &txid, int txoIndex) {
        std::string networkCode = network == "main" ? MAINNET_CODE : TESTNET_CODE;
        std::string url = CHAIN_SO_URL;
        url += "/";
        url += IS_TX_SPENT;
        url += "/";
        url += networkCode;
        url += "/";
        url += txid;
        url += "/";
        url += std::to_string(txoIndex);
        return url;
    }

    /**
     * Construct a "get_tx" URL
     *
     * @param network Which bitcoin network ('main' or 'test')
     * @param txid The bitcoin transaction id
     * @return The url
     */
    std::string getTxUrl(const std::string &network, const std::string &txid) {
        std::string networkCode = network == "main" ? MAINNET_CODE : TESTNET_CODE;
        std::string url = CHAIN_SO_URL;
        url += "/";
        url += GET_TX;
        url += "/";
        url += networkCode;
        url += "/";
        url += txid;
        return url;
    }

    /**
     * Parse a string containing JSON formatted data and return a Json::Value object.
     *
     * @param jsonData The string of data to parse
     * @return A Json::Value object
     */
    Json::Value parseJson(const std::string &jsonData) {
        Json::Value obj;
        std::istringstream iss(jsonData);
        iss >> obj;
        return obj;
    }

    UnspentData extractUnspentData(
            const Json::Value &obj, const std::string &address, int utxoIndex) {

        UnspentData unspentData;

        if (obj["status"].isNull() || obj["status"].asString() == "fail") {
            std::stringstream ss;
            ss << "Nothing found for address: " << address;
            throw std::runtime_error(ss.str());
        }

        // TODO: This is an incorrect use of utxoIndex!
        Json::Value tx = obj["data"]["txs"][utxoIndex];
        if (tx.type() == Json::nullValue) {
            std::stringstream ss;
            ss << "UTXO not found for utxoIndex: " << utxoIndex;
            throw std::runtime_error(ss.str());
        }

        unspentData.txid = tx["txid"].asString();
        unspentData.address = address;
        unspentData.scriptPubKeyHex = tx["script_hex"].asString();
        unspentData.amountSatoshis = btc2satoshi(atof(tx["value"].asString().data()));
        unspentData.utxoIndex = tx["output_no"].asInt();

        return unspentData;
    }


}

ChainSoQuery::~ChainSoQuery() = default;


/**
 * Using the given url, fetch the data from that url and return as a string. Chain.so has API rate limits,
 * so in the event of a failure message, we will retry periodically until we get the data.
 *
 * @param url The URL to get JSON data from
 * @param retryAttempt The number of retries done so far
 * @return The JSON data retrieved
 */
std::string ChainSoQuery::retrieveJsonData(const std::string &url, int retryAttempt) const {
    assert(retryAttempt > 0);
    CurlWrapper curl;
    sleep(1); // chain.so free API has rate limits, so pause here before every request
    std::string jsonData = curl.download(url);
    if (jsonData.find("Too many requests") != std::string::npos) {
        std::cerr << "Too many requests to chain.so. Sleeping for " << 5 * retryAttempt << " seconds...\n";
        sleep(5u * static_cast<unsigned int>(retryAttempt));
        return retrieveJsonData(url, retryAttempt + 1);
    }
    return jsonData;
}


/**
 * Given a BTC address and output index, return some data about the TX if it is unspent
 *
 * @param address The BTC address
 * @param utxoIndex The index of the unspent output
 * @param network The network being used ("main" or "test")
 * @return data about the TX
 */
UnspentData ChainSoQuery::getUnspentOutputs(
        const std::string &address, int utxoIndex, const std::string &network) const {

    std::string data = retrieveJsonData(getTxUnspentUrl(network, address));
    Json::Value obj = parseJson(data);

    return extractUnspentData(obj, address, utxoIndex);
}


/**
 * Given a transaction id and output index, follow the chain of transactions until an unspent
 * output is found. Return that output's txid.
 *
 * @param txid The transaction id
 * @param utxoIndex The output index
 * @param network The network being used ("main" or "test")
 * @return The txid for the unspent output
 */
std::string ChainSoQuery::getLastUpdatedTxid(
        const std::string &txid, int utxoIndex, const std::string &network) const {
    // check the output at the passed in txid and index. If it is spent, find what txid it was
    // spent to, and examine those outputs. recurse until an unspent output is found. return that txid.

    std::string url = isTxSpentUrl(network, txid, utxoIndex);
    std::string data = retrieveJsonData(url);
    Json::Value obj = parseJson(data);

    return extractLastUpdatedTxid(obj, txid, network);

}

/**
 * Given a txid (usually the "next" txid in a transaction chain) get all outputs for that
 * txid, and return the output index for the first non-OP_RETURN output.
 *
 * @param nextTxid The txid to examine
 * @param network Which bitcoin network ('main' or 'test')
 * @return The index number for the first non-OP_RETURN output
 */
int ChainSoQuery::determineNextUtxoIndex(const std::string &nextTxid, const std::string &network) const {

    // call getTx to find all the outputs for the next txid
    std::string url = getTxUrl(network, nextTxid);
    std::string data = retrieveJsonData(url);
    Json::Value obj = parseJson(data);

    if (obj["status"].isNull() || obj["status"].asString() == "fail") {
        std::stringstream ss;
        ss << "Nothing found for txid: " << nextTxid;
        throw std::runtime_error(ss.str());
    }

    Json::ArrayIndex numOutputs = obj["data"]["outputs"].size();

    // create set of output #s for all non OP_RETURN scripts
    std::set<int> outputNums;
    for (Json::ArrayIndex i = 0; i < numOutputs; i++) {
        Json::Value output = obj["data"]["outputs"][i];
        // only insert if this is not an OP_RETURN
        if (output["script"].asString().find("OP_RETURN", 0) == std::string::npos)
            outputNums.insert(output["output_no"].asInt());
    }

    if (outputNums.empty()) {
        std::stringstream ss;
        ss << "Child txid: " << nextTxid << " has too few output transactions. Can't follow tip.";
        throw std::runtime_error(ss.str());
    }

    //... return the index of the first non op_return output (smallest index)
    return *std::min_element(outputNums.begin(), outputNums.end());
}

/**
 * Given a JSON blob returned by querying if a TX is spent (see getLastUpdatedTxid()), extract
 * the next txid and possible output in the chain.
 * @param obj JSON returned from is_tx_spent()
 * @param txid the txid being examined
 * @param network Which bitcoin network ('main' or 'test')
 * @return The txid for the unspent output
 */
std::string
ChainSoQuery::extractLastUpdatedTxid(const Json::Value &obj, const std::string &txid,
                                     const std::string &network) const {

    if (obj["status"].isNull() || obj["status"].asString() == "fail") {
        std::stringstream ss;
        ss << "Nothing found for txid: " << txid;
        throw std::runtime_error(ss.str());
    }

    // if not spent, return current txid
    if (!obj["data"]["is_spent"].asBool()) {
        return txid;
    }

    // if spent, then we have to follow the transaction chain to find the "tip", or the
    // most-recent unspent transaction
    std::string nextTxid = obj["data"]["spent"]["txid"].asString();

    // the nextTxid may have more than one output--examine the outputs to get index of the
    // first non-OP_RETURN output.
    int nextUtxoIndex = determineNextUtxoIndex(nextTxid, network);

    // now, recurse to see if that output is spent or not
    return getLastUpdatedTxid(nextTxid, nextUtxoIndex, network);

}

