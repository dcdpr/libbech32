#ifndef BTCR_DID_DID_H
#define BTCR_DID_DID_H


#include "txref.h"
#include "../bitcoinRPCFacade.h"
#include <memory>
#include <string>

/**
 * This class represents a DID (decentralized identifier)
 */
class Did {

public:
    /**
     * Construct a DID from a did string (ex: did:btcr:xz4h-jzcl-rqpq-qjqxf09). Use
     * BitcoinRPCFacade to validate its existence.
     * @param didStr the did string
     * @param btc the BitcoinRPCFacade
     */
    Did(const std::string & didStr, const BitcoinRPCFacade & btc);

    /**
     * Get the Txref associated with this Did
     * @return the Txref
     */
    const std::shared_ptr<Txref> &getTxref() const;

private:
    std::shared_ptr<Txref> txref;
};


#endif //BTCR_DID_DID_H
