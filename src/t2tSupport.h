#ifndef TXREF_T2TSUPPORT_H
#define TXREF_T2TSUPPORT_H

#include "txid2txref.h"
#include "bitcoinRPCFacade.h"

namespace t2t {

    void encodeTxid(const BitcoinRPCFacade & btc, const Config & config, struct Transaction & transaction);

    void decodeTxref(const BitcoinRPCFacade & btc, const Config & config, struct Transaction & transaction);

}

#endif //TXREF_T2TSUPPORT_H
