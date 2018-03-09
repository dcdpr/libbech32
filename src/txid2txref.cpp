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
    std::string txid;
};


std::string find_homedir() {
    std::string ret;
    char * home = getenv("HOME");
    if(home != nullptr)
        ret.append(home);
    return ret;
}

int getConfig(int argc, char **argv, struct Config &config) {

//    auto opt = std::make_unique<AnyOption>();
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


    // print usage if no options
    if( ! opt->hasOptions()) {
        opt->printUsage();
        delete opt;
        return -1;
    }

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

    // finally, the last argument will be the txid
    if(opt->getArgc() < 1) {
        std::cerr << "txid not found. Check command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    config.txid = opt->getArgv(0);

    return 1;
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
        blockchaininfo_t blockChainInfo = btc.getblockchaininfo();

        // determine what network we are on
        bool isTestnet = blockChainInfo.chain == "test";

        // use txid to call getrawtransaction to find the blockhash

        getrawtransaction_t rawTransaction = btc.getrawtransaction(config.txid, 1);
        std::string blockHash = rawTransaction.blockhash;

        // use blockhash to call getblock to find the block height
        blockinfo_t blockInfo = btc.getblock(blockHash);
        int blockHeight = blockInfo.height;

        // warn if #confirmations are too low
        int numConfirmations = blockInfo.confirmations;
        if(numConfirmations < 6) {
            std::cout << "Warning: 6 confirmations are required for a valid txref: only "
                     << numConfirmations << " found.";
        }

        // go through block's transaction array to find transaction position
        std::vector<std::string> blockTransactions = blockInfo.tx;
        std::vector<std::string>::size_type blockIndex;
        for(blockIndex = 0; blockIndex < blockTransactions.size(); ++blockIndex) {
            std::string blockTxid = blockTransactions.at(blockIndex);
            if(blockTxid == config.txid)
                break;
        }

        if(blockIndex == blockTransactions.size()) {
            std::cerr << "Error: Could not find transaction " << config.txid
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

        std::cout << "txid: " << config.txid << "\n"
                  << "txref: " << txref << std::endl;


    }
    catch(BitcoinException &e)
    {
        if(e.getCode() == -5) {
            std::cerr << "Error: transaction " << config.txid << " not found." << std::endl;
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
