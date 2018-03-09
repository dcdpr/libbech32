#ifndef TXREF_BITCOINRPCFACADE_H
#define TXREF_BITCOINRPCFACADE_H

// Facade class that wraps the BitcoinApi objects

#include <string>

// forward decls from bitcoinaapi
class BitcoinAPI;
struct getrawtransaction_t;
struct blockinfo_t;

// struct for local impl of getblockchaininfo()
struct blockchaininfo_t {
    std::string chain;
    int blocks;
    int headers;
    std::string bestblockhash;
    double difficulty;
    int mediantime;
    double verificationprogress;
    std::string chainwork;
    bool pruned;
    int pruneheight;
    //std::vector<...> softforks;
    //std::vector<...> bip9_softforks;
};


class BitcoinRPCFacade {

private:
    BitcoinAPI *bitcoinAPI;

public:

    BitcoinRPCFacade(
            const std::string& user,
            const std::string& password,
            const std::string& host,
            int port);

    // forwards to existing bitcoinapi functions
    virtual getrawtransaction_t getrawtransaction(const std::string& txid, int verbose);
    virtual blockinfo_t getblock(const std::string& blockhash);

    // implement missing bitcoinapi functions
    virtual blockchaininfo_t getblockchaininfo();

};


#endif //TXREF_BITCOINRPCFACADE_H
