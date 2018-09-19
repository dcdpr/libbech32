#ifndef TXREF_CHAINSOQUERY_H
#define TXREF_CHAINSOQUERY_H


#include "chainQuery.h"

namespace Json {
    class Value;
}

class ChainSoQuery : public ChainQuery {

public:
    virtual ~ChainSoQuery() override;

    /**
     * Given a BTC address and output index, return some data about the TX if it is unspent
     *
     * @param address The BTC address
     * @param utxoIndex The index of the unspent output
     * @param network The network being used ("main" or "test")
     * @return data about the TX
     */
    UnspentData
    getUnspentOutputs(
            const std::string & address,
            int utxoIndex,
            const std::string & network) const override;

    /**
     * Given a transaction id and output index, follow the chain of transactions until an unspent
     * output is found. Return that output's txid.
     *
     * @param txid The transaction id
     * @param utxoIndex The output index
     * @param network The network being used ("main" or "test")
     * @return The txid for the unspent output
     */
    std::string
    getLastUpdatedTxid(
            const std::string &txid,
            int utxoIndex,
            const std::string & network) const override;

protected:

   /**
    * Using the given url, fetch the data from that url and return as a string. Chain.so has API rate limits,
    * so in the event of a failure message, we will retry periodically until we get the data.
    *
    * @param url The URL to get JSON data from
    * @param retryAttempt The number of retries done so far
    * @return The JSON data retrieved
    */
    virtual std::string retrieveJsonData(const std::string & url, int retryAttempt = 1) const;

   /**
    * Given a txid (usually the "next" txid in a transaction chain) get all outputs for that
    * txid, and return the output index for the first non-OP_RETURN output.
    *
    * @param nextTxid The txid to examine
    * @param network Which bitcoin network ('main' or 'test')
    * @return The index number for the first non-OP_RETURN output
    */
    virtual int determineNextUtxoIndex(const std::string & nextTxid, const std::string & network) const;

    /**
     * Given a JSON blob returned by querying if a TX is spent (see getLastUpdatedTxid()), extract
     * the next txid and possible output in the chain.
     * @param obj JSON returned from is_tx_spent()
     * @param txid the txid being examined
     * @param network Which bitcoin network ('main' or 'test')
     * @return The txid for the unspent output
     */
    virtual std::string extractLastUpdatedTxid(const Json::Value &obj, const std::string &txid, const std::string &network) const;

};


#endif //TXREF_CHAINSOQUERY_H
