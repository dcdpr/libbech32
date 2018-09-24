#ifndef BTCR_DID_TXREF_H
#define BTCR_DID_TXREF_H

#include "vout.h"
#include "txid.h"
#include "blockHeight.h"
#include "transactionPosition.h"
#include "../bitcoinRPCFacade.h"
#include <memory>

/**
 * This class represents a TXREF: a transaction reference ...
 */
class Txref {

public:
    /**
     * Construct a Txref from a Txid and Vout. Use BitcoinRPCFacade to
     * validate its existence.
     * @param txid the Txid for the transaction
     * @param vout the Vout for the output being referenced
     * @param btc the BitcoinRPCFacade
     */
    Txref(const Txid & txid, const Vout & vout, const BitcoinRPCFacade & btc);

    /**
     * Construct a Txref from a txref string. Use BitcoinRPCFacade to
     * validate its existence.
     * @param txref the txref string
     * @param btc the BitcoinRPCFacade
     */
    Txref(const std::string & txref, const BitcoinRPCFacade & btc);

    /**
     * Get this Txref as a string
     * @return this Txref as a string
     */
    std::string asString();

    /**
     * Get the Txid associated with this Txref
     * @return the Txid
     */
    const std::shared_ptr<Txid> &getTxid() const;

    /**
     * Get the Vout associated with this Txref
     * @return the Vout
     */
    const std::shared_ptr<Vout> &getVout() const;

private:
    std::shared_ptr<Txid> txid;
    std::shared_ptr<Vout> vout;

    std::string txrefStr;

    /**
     * Verify that the transaction referred to by the Txid has enough Vouts
     */
    bool verifyVoutForTxid(const BitcoinRPCFacade & btc);
};


#endif //BTCR_DID_TXREF_H
