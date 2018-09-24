#include <memory>
#include <stdexcept>
#include <bitcoinapi/types.h>
#include <iostream>
#include "txid.h"

Txid::Txid(const std::string & inTxidStr, const BitcoinRPCFacade & btc) {
    if(!isInputStringValid(inTxidStr))
        throw std::runtime_error("input string not valid");

    if(!existsInNetwork(inTxidStr, btc))
        throw std::runtime_error("txid does not exist");

    extractTransactionDetails(inTxidStr, btc);

    txidStr = inTxidStr;
}

bool Txid::isInputStringValid(const std::string & inTxidStr) const {
    return inTxidStr.length() == 64 &&
            inTxidStr.find_first_not_of("0123456789abcdef") == std::string::npos;
}

bool Txid::existsInNetwork(const std::string & inTxidStr, const BitcoinRPCFacade & btc) const {
    // use txid to call getrawtransaction
    getrawtransaction_t rawTransaction = btc.getrawtransaction(inTxidStr, 0);
    return !rawTransaction.hex.empty();
}

std::string Txid::asString() const {
    return txidStr;
}

std::shared_ptr<BlockHeight> Txid::blockHeight() const {
    return pBlockHeight;
}

std::shared_ptr<TransactionPosition> Txid::transactionPosition() const {
    return pTransactionPosition;
}

void Txid::extractTransactionDetails(const std::string & inTxidStr, const BitcoinRPCFacade & btc) {

    // use txid to call getrawtransaction to find the blockhash
    getrawtransaction_t rawTransaction = btc.getrawtransaction(inTxidStr, 1);
    std::string blockHash = rawTransaction.blockhash;

    // use blockhash to call getblock to find the block height
    blockinfo_t blockInfo = btc.getblock(blockHash);
    int blockHeight = blockInfo.height;

    pBlockHeight = std::make_shared<BlockHeight>(blockHeight);

    // TODO warn if #confirmations are too low

    // determine what network we are on
    blockchaininfo_t blockChainInfo = btc.getblockchaininfo();
    testnet = blockChainInfo.chain == "test";

    // go through block's transaction array to find transaction position
    std::vector<std::string> blockTransactions = blockInfo.tx;
    std::vector<std::string>::size_type blockIndex;
    for (blockIndex = 0; blockIndex < blockTransactions.size(); ++blockIndex) {
        std::string blockTxid = blockTransactions.at(blockIndex);
        if (blockTxid == inTxidStr)
            break;
    }

    if (blockIndex == blockTransactions.size()) {
        throw std::runtime_error("Could not find transaction " + inTxidStr + "within the block");
    }

    pTransactionPosition = std::make_shared<TransactionPosition>(blockIndex);

}

bool Txid::isTestnet() const {
    return testnet;
}

bool Txid::operator==(const Txid &rhs) const {
    return *pBlockHeight == *rhs.pBlockHeight &&
           *pTransactionPosition == *rhs.pTransactionPosition &&
           testnet == rhs.testnet &&
           txidStr == rhs.txidStr;
}

bool Txid::operator!=(const Txid &rhs) const {
    return !(rhs == *this);
}

