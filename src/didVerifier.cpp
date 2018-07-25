#include <iostream>
#include <cstdlib>
#include <memory>
#include "bitcoinRPCFacade.h"
#include "chainQuery.h"
#include "chainSoQuery.h"
#include "encodeOpReturnData.h"
#include "satoshis.h"
#include "classifyInputString.h"
#include "txref.h"
#include "txid2txref.h"
#include "t2tSupport.h"
#include <bitcoinapi/bitcoinapi.h>
#include "anyoption.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;


struct TransactionData {
    std::string inputString;
    std::string outputAddress;
    std::string privateKey;
    std::string ddoRef;
    double fee = 0.0;
    int txoIndex = 0;
};


std::string find_homedir() {
    std::string ret;
    char * home = getenv("HOME");
    if(home != nullptr)
        ret.append(home);
    return ret;
}

int parseCommandLineArgs(int argc, char **argv, struct t2t::Config &config, struct TransactionData &transactionData) {

    auto opt = new AnyOption();
    opt->setFileDelimiterChar('=');

    opt->addUsage( "" );
    opt->addUsage( "Usage: createBtcrDid [options] <inputXXX> <changeAddress> <network> <WIF> <fee> <ddoRef>" );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help                 Print this help " );
    opt->addUsage( " --rpchost [rpchost or IP]  RPC host (default: 127.0.0.1) " );
    opt->addUsage( " --rpcuser [user]           RPC user " );
    opt->addUsage( " --rpcpassword [pass]       RPC password " );
    opt->addUsage( " --rpcport [port]           RPC port (default: try both 8332 and 18332) " );
    opt->addUsage( " --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf) " );
    opt->addUsage( "" );
    opt->addUsage( "<did>                       the BTCR DID to verify. Could be txref or txref-ext based" );

    opt->setFlag("help", 'h');
    opt->setCommandOption("rpchost");
    opt->setOption("rpcuser");
    opt->setOption("rpcpassword");
    opt->setOption("rpcport");
    opt->setCommandOption("config");

    // parse any command line arguments--this is a first pass, mainly to get a possible
    // "config" option that tells if the bitcoin.conf file is in a non-default location
    opt->processCommandArgs( argc, argv );

    // print usage if no options
    if( ! opt->hasOptions()) {
        opt->printUsage();
        delete opt;
        return 0;
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

    // get the positional arguments
    if(opt->getArgc() < 1) {
        std::cerr << "Error: all required arguments not found. Check command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    transactionData.inputString = opt->getArgv(0);
    config.query = opt->getArgv(0); // TODO do we need this in two places?

    // TODO validate position arguments

    return 1;
}


int main(int argc, char *argv[]) {

    struct t2t::Config config;
    struct TransactionData transactionData;

    int ret = parseCommandLineArgs(argc, argv, config, transactionData);
    if (ret < 1) {
        std::exit(ret);
    }


    try {

        BitcoinRPCFacade btc(config.rpcuser, config.rpcpassword, config.rpchost, config.rpcport);

        blockchaininfo_t blockChainInfo = btc.getblockchaininfo();

        // 0. Determine InputType

        InputParam inputParam = classifyInputString(transactionData.inputString);

            // skeleton of a diddo verifier:
    //
    // 1) verify that diddo is valid json-ld (how?)
    // 2) extract txref from diddo (id field?)
    // 3) decode txref to find block height, transaction position, and txo index
    // 4) follow txo chain if necessary (as in didResolver) to find the last unspent output
    // 5) Extract the hex-encoded public key that signed the transaction and verify it matches
    //    the #keys-1 key in the authentication array
    // 6) verify any signatures embeded in the other parts of the diddo
    // 7) what else do we need to verify? urls? endpoints? references to x? what else can be signed?

    }
    catch(BitcoinException &e)
    {
        std::cerr << e.getCode() << " " << e.getMessage() << std::endl;
        std::exit(-1);
    }
    catch(std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        std::exit(-1);
    }

}
