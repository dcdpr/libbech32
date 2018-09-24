#ifndef TXREF_T2TSUPPORT_H
#define TXREF_T2TSUPPORT_H

#include "txid2txref.h"
#include "bitcoinRPCFacade.h"

namespace t2t {

    struct ConfigTemp {        // used for now so things compile as I'm working out the new domain objects
        std::string query ="";
        int txoIndex = 0;
        bool forceExtended = false;
    };

    void encodeTxid(const BitcoinRPCFacade & btc, const ConfigTemp & config, struct Transaction & transaction);

    void decodeTxref(const BitcoinRPCFacade & btc, const ConfigTemp & config, struct Transaction & transaction);

}

#endif //TXREF_T2TSUPPORT_H
