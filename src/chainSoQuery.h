#ifndef TXREF_CHAINSOQUERY_H
#define TXREF_CHAINSOQUERY_H


#include "chainQuery.h"

class ChainSoQuery : public ChainQuery {

public:
    ~ChainSoQuery() override = default;

    UnspentData getUnspentOutputs(
            const std::string & address, int utxoIndex, const std::string & network) const override;

protected:

    virtual std::string retrieveJsonData(const std::string & url) const;

    virtual UnspentData populateFromJson(
            const std::string & address, int utxoIndex, const std::string & jsonData) const;
};


#endif //TXREF_CHAINSOQUERY_H
