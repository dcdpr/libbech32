#include <iostream>
#include <cstdlib>
#include <memory>
#include "bitcoinRPCFacade.h"
#include "chainQuery.h"
#include "chainSoQuery.h"
#include "encodeOpReturnData.h"
#include "satoshis.h"
#include "classifyInputString.h"
#include "txid2txref.h"
#include "t2tSupport.h"
#include <bitcoinapi/bitcoinapi.h>
#include "anyoption.h"
#include "domain/did.h"
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

// global vars set by some "secret" testing command-line options
namespace testing {
    static bool exitAfterFollowTip = false;
}


int parseCommandLineArgs(int argc, char **argv,
                         struct RpcConfig &rpcConfig,
                         struct TransactionData &transactionData) {

    auto opt = new AnyOption();
    opt->setFileDelimiterChar('=');

    opt->addUsage( "" );
    opt->addUsage( "Usage: didResolver [options] <did>" );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help                 Print this help " );
    opt->addUsage( " --rpchost [rpchost or IP]  RPC host (default: 127.0.0.1) " );
    opt->addUsage( " --rpcuser [user]           RPC user " );
    opt->addUsage( " --rpcpassword [pass]       RPC password " );
    opt->addUsage( " --rpcport [port]           RPC port (default: try both 8332 and 18332) " );
    opt->addUsage( " --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf) " );
    opt->addUsage( "" );
    opt->addUsage( "<did>                       the BTCR DID to resolve. Could be txref or txref-ext based" );

    opt->setFlag("help", 'h');
    opt->setCommandOption("rpchost");
    opt->setOption("rpcuser");
    opt->setOption("rpcpassword");
    opt->setOption("rpcport");
    opt->setCommandOption("config");

    // "secret" testing flags
    opt->setFlag("exitAfterFollowTip", 'f');


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

    // check for some "secret" arguments that are used to test some operations
    if (opt->getFlag("exitAfterFollowTip") || opt->getFlag('f')) {
        testing::exitAfterFollowTip = true;
    }


    // get the positional arguments
    if(opt->getArgc() < 1) {
        std::cerr << "Error: all required arguments not found. Check command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }

    transactionData.inputString = opt->getArgv(0);

    // TODO validate position arguments

    return 1;
}


int main(int argc, char *argv[]) {

    struct RpcConfig rpcConfig;
    struct TransactionData transactionData;

    int ret = parseCommandLineArgs(argc, argv, rpcConfig, transactionData);
    if (ret < 1) {
        std::exit(ret);
    }

    try {

        BitcoinRPCFacade btc(rpcConfig);

        // create Did
        Did did(transactionData.inputString, btc);

        auto pTxref = did.getTxref();

        std::cout << "Valid txref found:\n";
        std::cout << "  txref: " << pTxref->asString() << "\n";
        std::cout << "  txid: " << pTxref->getTxid()->asString() << "\n";
        std::cout << "  block height: " << pTxref->getTxid()->blockHeight()->value() << "\n";
        std::cout << "  transaction position: " << pTxref->getTxid()->transactionPosition()->value() << "\n";
        std::cout << "  txoIndex: " << pTxref->getVout()->value() << "\n";


        // 4) Is txo at txoIndex unspent?

        utxoinfo_t utxoinfo = btc.gettxout(pTxref->getTxid()->asString(), pTxref->getVout()->value());

        // TODO hmm, if btc.gettxout() returns anything, then it is unspent. If it returns nothing,
        // that means it is spent, or possibly an op_return output

        std::string txidForDID;

        if(!utxoinfo.bestblock.empty() && utxoinfo.confirmations != 0) {
            // yes: this is the latest version of the DID. From this we can construct the DID Document
            std::cout << "Last txid with unspent output: " << pTxref->getTxid()->asString() << "\n";
            txidForDID = pTxref->getTxid()->asString();
        }
        else {
            // no : recursively follow transaction chain until txo  with an unspent output is found
            ChainQuery *q = new ChainSoQuery();
            std::string lastTxid =
                    q->getLastUpdatedTxid(
                            pTxref->getTxid()->asString(),
                            pTxref->getVout()->value(),
                            pTxref->getTxid()->isTestnet() ? "test" :"main");
            std::cout << "Last txid with unspent output: " << lastTxid << "\n";
            txidForDID = lastTxid;
        }

        if(testing::exitAfterFollowTip) {
            exit(0);
        }



        // 5) Extract the hex-encoded public key that signed the transaction and update the DID document
        //    with default authentication capability
        // 6) Populate the first entry of the publicKey array in the DID document. This uses the
        //    Koblitz Elliptic Curve Signature 2016 signature suite
        // 7) Populate the first entry of the authentication array in the DID document, referencing
        //    the key above. Note: It is a BTCR method convention that #keys-1 corresponds to the
        //    transaction signing key.
        // 8) If the transaction contains an OP_RETURN field, populate the serviceEndpoint in the
        //    DID document. This is assumed to reference supplementary DID document data
        //    -- Add an entry to the service section of the DID document, type is BTCREndpoint
        //    -- serviceEndpoint is the value in the OP_RETURN field, e.g.
        //       "https://github.com/myopreturnpointer"
        //    -- timestamp is XXX?
        // 9) Add SatoshiAuditTrail ?

        // IF there was OP_RETURN data, proceed to next phase

        // 10) Retrieve the jsonld document from serviceEndpoint.BTCREndpoint
        //     -- how to handle errors? what if it is not there? what if it is not json-ld? how to tell?
        // 11) Authenticate this JSON-LD fragment as valid
        //     -- fragment must have signature matching Bitcoin transaction signing key
        //     -- UNLESS the fragment is stored on an immutable data store, like IPFS... then what?
        // 12) Merge in known JSON-LD values (additional keys, authorizations, etc) as appropriate
        //     into DID document. Additive only!
        //     -- Any fields that are part of the DID specification (publicKey, authentication,
        //        service) will be merged into the DID document by appending their entries to the
        //        arrays of the appropriate field
        //     -- If continuation overwrites the Bitcoin key value, ERROR
        //     -- Unknown JSON-LD values (in type "DID Document") are appended to the constructed
        //        DID Document
        //     -- Non-DID JSON-LD data types that may also be at BTCEndpoint (such as Verifiable
        //        Claims) are ignored by resolver
        // 13) incorporate "patch" documents?
        // 14) wrap output with "resolver envelope"?


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
