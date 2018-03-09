#include <iostream>
#include <cstdlib>
#include <memory>
#include "BitcoinRPCFacade.h"
#include <libtxref/txref.h>
#include <bitcoinapi/bitcoinapi.h>
#include <anyoption/anyoption.h>

struct Config {
    std::string rpcuser;
    std::string rpcpassword;
    std::string rpchost = "127.0.0.1";
    int rpcport = 0;
    std::string query;
};

struct Transaction {
    std::string txid;
    std::string txref;
    std::string network;
    int height;
    int position;
    std::string query;
};


std::string find_homedir() {
    std::string ret;
    char * home = getenv("HOME");
    if(home != nullptr)
        ret.append(home);
    return ret;
}

int getConfig(int argc, char **argv, struct Config &config) {

    auto opt = new AnyOption();
    opt->setFileDelimiterChar('=');

    opt->addUsage( "" );
    opt->addUsage( "Usage: txid2txref [options] <txid>" );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help                 Print this help " );
    opt->addUsage( " --rpchost [rpchost or IP]  RPC host (default: 127.0.0.1) " );
    opt->addUsage( " --rpcuser [user]           RPC user " );
    opt->addUsage( " --rpcpassword [pass]       RPC password " );
    opt->addUsage( " --rpcport [port]           RPC port (default: try both 8332 and 18332) " );
    opt->addUsage( " --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf) " );
    opt->addUsage( "" );

    opt->setFlag("help", 'h');
    opt->setCommandOption("rpchost");
    opt->setOption("rpcuser");
    opt->setOption("rpcpassword");
    opt->setOption("rpcport");
    opt->setCommandOption("config");

    // parse any command line arguments--this is to get the "config" option
    opt->processCommandArgs( argc, argv );

        // print usage if no options
    if( ! opt->hasOptions()) {
        opt->printUsage();
        delete opt;
        return -1;
    }

    // see if there is a bitcoin.conf file to parse. If not, continue.
    if (opt->getValue("config") != nullptr) {
        opt->processFile(opt->getValue("config"));
    }
    else {
        std::string home = find_homedir();
        if(!home.empty()) {
            std::string configPath = home + "/.bitcoin/bitcoin.conf";
            if(!opt->processFile(configPath.data())) {
                std::cerr << "Warning: Config file " << configPath
                          << " not readable. Perhaps try --config? Attempting to continue..."
                          << std::endl;
            }
        }
    }

    // parse command line arguments AGAIN--this is because command line args should override config file
    opt->processCommandArgs( argc, argv );


    // print usage if help was requested
    if (opt->getFlag("help") || opt->getFlag('h')) {
        opt->printUsage();
        delete opt;
        return 0;
    }

    // see if there is an rpchost specified. If not, use default
    if (opt->getValue("rpchost") != nullptr) {
        config.rpchost = opt->getValue("rpchost");
    }

    // see if there is an rpcuser specified. If not, exit
    if (opt->getValue("rpcuser") == nullptr) {
        std::cerr << "'rpcuser' not found. Check bitcoin.conf or command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    config.rpcuser = opt->getValue("rpcuser");

    // see if there is an rpcpassword specified. If not, exit
    if (opt->getValue("rpcpassword") == nullptr) {
        std::cerr << "'rpcpassword' not found. Check bitcoin.conf or command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    config.rpcpassword = opt->getValue("rpcpassword");

    // will try both well known ports (8332 and 18332) if one is not specified
    if (opt->getValue("rpcport") != nullptr) {
        config.rpcport = std::atoi(opt->getValue("rpcport"));
    }

    // finally, the last argument will be the query string -- either the txid or the txref
    if(opt->getArgc() < 2) {
        std::cerr << "txid/txref not found. Check command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    config.query = opt->getArgv(1);

    return 1;
}

void encodeTxid(const BitcoinRPCFacade & btc, Config & config, struct Transaction & transaction) {

    blockchaininfo_t blockChainInfo = btc.getblockchaininfo();

    // determine what network we are on
    bool isTestnet = blockChainInfo.chain == "test";

    // use txid to call getrawtransaction to find the blockhash
    getrawtransaction_t rawTransaction = btc.getrawtransaction(config.query, 1);
    std::string blockHash = rawTransaction.blockhash;

    // use blockhash to call getblock to find the block height
    blockinfo_t blockInfo = btc.getblock(blockHash);
    int blockHeight = blockInfo.height;

    // warn if #confirmations are too low
    int numConfirmations = blockInfo.confirmations;
    if(numConfirmations < 6) {
        std::cout << "Warning: 6 confirmations are required for a valid txref: only "
                  << numConfirmations << " found." << std::endl;
    }

    // go through block's transaction array to find transaction position
    std::vector<std::string> blockTransactions = blockInfo.tx;
    std::vector<std::string>::size_type blockIndex;
    for(blockIndex = 0; blockIndex < blockTransactions.size(); ++blockIndex) {
        std::string blockTxid = blockTransactions.at(blockIndex);
        if(blockTxid == config.query)
            break;
    }

    if(blockIndex == blockTransactions.size()) {
        std::cerr << "Error: Could not find transaction " << config.query
                  << " within the block." << std::endl;
        std::exit(-1);
    }

    // call txref code with block height and trans pos to get txref
    std::string txref;
    if(isTestnet) {
        txref = txref::bitcoinTxrefEncodeTestnet(
                txref::BECH32_HRP_TEST, txref::MAGIC_BTC_TEST,
                blockHeight, static_cast<int>(blockIndex));
    }
    else {
        txref = txref::bitcoinTxrefEncode(
                txref::BECH32_HRP_MAIN, txref::MAGIC_BTC_MAIN,
                blockHeight, static_cast<int>(blockIndex));
    }

    // output
    transaction.query = config.query;
    transaction.txid = config.query;
    transaction.txref = txref;
    transaction.height = blockHeight;
    transaction.position = static_cast<int>(blockIndex);
    transaction.network = blockChainInfo.chain;
}

void decodeTxref(const BitcoinRPCFacade & btc, Config & config, struct Transaction & transaction) {

    txref::LocationData locationData = txref::bitcoinTxrefDecode(config.query);

    blockchaininfo_t blockChainInfo = btc.getblockchaininfo();

    // determine what network we are on
    bool isTestnet = blockChainInfo.chain == "test";


    // get block hash for block at location "height"
    std::string blockHash = btc.getblockhash(locationData.blockHeight);

    // use block hash to get the block
    blockinfo_t blockInfo = btc.getblock(blockHash);

    // get the txid from the transaction at "position"
    std::string txid;
    try {
        txid = blockInfo.tx.at(locationData.transactionPosition);
    }
    catch(std::out_of_range &e) {
        std::cerr << "Error: Could not find transaction " << config.query
                  << " within the block." << std::endl;
        std::exit(-1);
    }

    // output
    transaction.query = config.query;
    transaction.txid = txid;
    transaction.txref = config.query;
    transaction.height = locationData.blockHeight;
    transaction.position = locationData.transactionPosition;
    transaction.network = blockChainInfo.chain;
}

int main(int argc, char *argv[]) {

    struct Config config;

    int ret = getConfig(argc, argv, config);
    if(ret < 1) {
        std::exit(ret);
    }

    try
    {
        BitcoinRPCFacade btc(config.rpcuser, config.rpcpassword, config.rpchost, config.rpcport);

        Transaction transaction;

        if(config.query.length() == 64) {
            encodeTxid(btc, config, transaction);
            std::cout << "txid: " << config.query << "\n"
                      << "txref: " << transaction.txref << std::endl;
        }
        else {
            decodeTxref(btc, config, transaction);
            std::cout << "txref: " << config.query << "\n"
                      << "txid: " << transaction.txid << std::endl;
        }

    }
    catch(BitcoinException &e)
    {
        if(e.getCode() == -5) {
            std::cerr << "Error: transaction " << config.query << " not found." << std::endl;
            std::exit(-1);
        }

        std::cerr << e.getCode() << " " << e.getMessage() << std::endl;
        std::exit(-1);
    }
    catch(std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }

    return 0;
}
