#ifndef TXREF_CHAINQUERY_H
#define TXREF_CHAINQUERY_H

#include <string>

struct UnspentData {
    std::string address;
    std::string txid;
    std::string scriptPubKeyHex;
    int64_t amountSatoshis = 0;
    int index = -1;
};


class ChainQuery {

public:
    virtual UnspentData getUnspentOutputs(
            const std::string & address, int transactionIndex, const std::string & network) const = 0;

};


#endif //TXREF_CHAINQUERY_H
