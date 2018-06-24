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
    virtual ~ChainQuery() = default;

    virtual UnspentData getUnspentOutputs(
            const std::string & address, int utxoIndex, const std::string & network) const = 0;
};


#endif //TXREF_CHAINQUERY_H
