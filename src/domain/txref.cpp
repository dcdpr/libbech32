#include <memory>

#include <memory>

#include <memory>

#include <stdexcept>
#include <bitcoinapi/types.h>
#include <iostream>
#include "txrefCodec.h"
#include "txref.h"

Txref::Txref(const Txid & t, const Vout & v, const BitcoinRPCFacade & btc)
{
    // make copies of passed in parameters
    txid = std::make_shared<Txid>(t);
    vout = std::make_shared<Vout>(v);

    if(!verifyVoutForTxid(btc)) {
        throw std::runtime_error("vout provided is too large for this transaction");
    }

    // call txref encode with block height, transaction position, and vout to get txref string
    if (txid->isTestnet()) {
        txrefStr = txref::encodeTestnet(
                txid->blockHeight()->value(),
                txid->transactionPosition()->value(),
                vout->value(),
                true); // TODO forceExtended = true for now
    } else {
        txrefStr = txref::encode(
                txid->blockHeight()->value(),
                txid->transactionPosition()->value(),
                vout->value(),
                true); // TODO forceExtended = true for now
    }
}

Txref::Txref(const std::string &t, const BitcoinRPCFacade &btc) {

    // make copies of passed in parameters
    txrefStr = t;

    txref::LocationData locationData = txref::decode(txrefStr);


    // get block hash for block at location "blockHeight"
    std::string blockHash = btc.getblockhash(locationData.blockHeight);

    // use block hash to get the block
    blockinfo_t blockInfo = btc.getblock(blockHash);

    // get the txid from the transaction at "position"
    std::string txidStr;
    try {
        txidStr = blockInfo.tx.at(
                static_cast<unsigned long>(locationData.transactionPosition));
    }
    catch (std::out_of_range &) {
        std::cerr << "Error: Could not find transaction " << txrefStr
                  << " within the block." << std::endl;
        std::exit(-1);
    }

    txid = std::make_shared<Txid>(txidStr, btc);
    vout = std::make_shared<Vout>(locationData.txoIndex);

}

std::string Txref::asString() {
    return txrefStr;
}

bool Txref::verifyVoutForTxid(const BitcoinRPCFacade & btc) {
    // use txid to call getrawtransaction to find the blockhash
    getrawtransaction_t rawTransaction = btc.getrawtransaction(txid->asString(), 1);
    return rawTransaction.vout.size() >= static_cast<std::vector<vout_t>::size_type >(vout->value());
}

const std::shared_ptr<Txid> &Txref::getTxid() const {
    return txid;
}

const std::shared_ptr<Vout> &Txref::getVout() const {
    return vout;
}

