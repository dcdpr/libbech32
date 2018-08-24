#ifndef TXREF_CHAINQUERY_H
#define TXREF_CHAINQUERY_H

#include <string>

struct UnspentData {
    std::string address;
    std::string txid;
    std::string scriptPubKeyHex;
    int64_t amountSatoshis = 0;
    int utxoIndex = 0;
};


class ChainQuery {
public:
    virtual ~ChainQuery();

    /**
     * Given a BTC address and output index, return some data about the TX if it is unspent
     *
     * @param address The BTC address
     * @param utxoIndex The index of the unspent output
     * @param network The network being used ("main" or "test")
     * @return data about the TX
     */
    virtual UnspentData
    getUnspentOutputs(
            const std::string & address,
            int utxoIndex,
            const std::string & network) const = 0;

    /**
     * Given a transaction id and output index, follow the chain of transactions until an unspent
     * output is found. Return that output's txid.
     *
     * @param txid The transaction id
     * @param utxoIndex The output index
     * @param network The network being used ("main" or "test")
     * @return The txid for the unspent output
     */
    virtual std::string
    getLastUpdatedTxid(
            const std::string &txid,
            int utxoIndex,
            const std::string & network) const = 0;
};


#endif //TXREF_CHAINQUERY_H
