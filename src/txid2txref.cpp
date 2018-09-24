#include <iostream>
#include <cstdlib>
#include <memory>
#include "txid2txref.h"
#include "t2tSupport.h"
#include "bitcoinRPCFacade.h"
#include <bitcoinapi/bitcoinapi.h>
#include "anyoption.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

struct CmdlineInput {
    std::string query ="";
    int txoIndex = 0;
    bool forceExtended = false;
};


void printAsJson(const t2t::Transaction &transaction) {
    pt::ptree root;

    root.put("txid", transaction.txid);
    root.put("txref", transaction.txref);
    root.put("network", transaction.network);
    root.put("block-height", transaction.blockHeight);
    root.put("transaction-position", transaction.position);
    root.put("txo-index", transaction.txoIndex);
    root.put("query-string", transaction.query);

    pt::write_json(std::cout, root);
}

std::string find_homedir() {
    std::string ret;
    char * home = getenv("HOME");
    if(home != nullptr)
        ret.append(home);
    return ret;
}

int parseCommandLineArgs(int argc, char **argv,
                         struct RpcConfig &rpcConfig,
                         struct CmdlineInput &config) {

    auto opt = new AnyOption();
    opt->setFileDelimiterChar('=');

    opt->addUsage( "" );
    opt->addUsage( "Usage: txid2txref [options] <txid|txref|txref-ext>" );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help                 Print this help " );
    opt->addUsage( " --rpchost [rpchost or IP]  RPC host (default: 127.0.0.1) " );
    opt->addUsage( " --rpcuser [user]           RPC user " );
    opt->addUsage( " --rpcpassword [pass]       RPC password " );
    opt->addUsage( " --rpcport [port]           RPC port (default: try both 8332 and 18332) " );
    opt->addUsage( " --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf) " );
    opt->addUsage( " --txoIndex [index #]       Index # for TXO within the transaction (default: 0) " );
    opt->addUsage( "" );
    opt->addUsage( "<txid|txref|txref-ext>      input: can be a txid to encode, or a txref or txref-ext to decode" );

    opt->setFlag("help", 'h');
    opt->setCommandOption("rpchost");
    opt->setOption("rpcuser");
    opt->setOption("rpcpassword");
    opt->setOption("rpcport");
    opt->setCommandOption("config");
    opt->setOption("txoIndex");

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
        rpcConfig.rpchost = opt->getValue("rpchost");
    }

    // see if there is an rpcuser specified. If not, exit
    if (opt->getValue("rpcuser") == nullptr) {
        std::cerr << "'rpcuser' not found. Check bitcoin.conf or command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    rpcConfig.rpcuser = opt->getValue("rpcuser");

    // see if there is an rpcpassword specified. If not, exit
    if (opt->getValue("rpcpassword") == nullptr) {
        std::cerr << "'rpcpassword' not found. Check bitcoin.conf or command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    rpcConfig.rpcpassword = opt->getValue("rpcpassword");

    // will try both well known ports (8332 and 18332) if one is not specified
    if (opt->getValue("rpcport") != nullptr) {
        rpcConfig.rpcport = std::atoi(opt->getValue("rpcport"));
    }

    // see if a txoIndex was provided. If so, make sure to force generation of extended txref
    if (opt->getValue("txoIndex") != nullptr) {
        config.txoIndex = std::atoi(opt->getValue("txoIndex"));
        config.forceExtended = true;
    }

    // finally, the last argument will be the query string -- either the txid or the txref
    if(opt->getArgc() < 1) {
        std::cerr << "txid/txref not found. Check command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    config.query = opt->getArgv(0);

    return 1;
}

int main(int argc, char *argv[]) {

    struct RpcConfig rpcConfig;
    struct CmdlineInput cmdlineInput;

    int ret = parseCommandLineArgs(argc, argv, rpcConfig, cmdlineInput);
    if(ret < 1) {
        std::exit(ret);
    }

    try
    {
        BitcoinRPCFacade btc(rpcConfig);

        t2t::Transaction transaction;

        // TODO temporary
        t2t::ConfigTemp configTemp;
        configTemp.query = cmdlineInput.query;
        configTemp.txoIndex = cmdlineInput.txoIndex;
        configTemp.forceExtended = cmdlineInput.forceExtended;


        if(cmdlineInput.query.length() == 64) {
            t2t::encodeTxid(btc, configTemp, transaction);
        }
        else {
            t2t::decodeTxref(btc, configTemp, transaction);
        }

        printAsJson(transaction);

    }
    catch(BitcoinException &e)
    {
        if(e.getCode() == -5) {
            std::cerr << "Error: transaction " << cmdlineInput.query << " not found." << std::endl;
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

