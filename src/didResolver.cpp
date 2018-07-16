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

        // skeleton of a resolver:
        //
        // 1) confirm DID is a btcr DID. fail if not

        const std::string schemeAndMethod = "did:btcr:";

        std::string did = transactionData.inputString;

        if(did.find(schemeAndMethod) != 0) {
            throw std::runtime_error("DID parameter not a valid BTCR DID. Should be of the form 'did:btcr:<txref>'");
        }

        // 2) extract txref from DID

        did.erase(0, schemeAndMethod.length());

        InputParam inputParam = classifyInputString(did);

        if(inputParam != txref_param && inputParam != txrefext_param) {
            throw std::runtime_error("DID parameter doesn't contain a valid txref. Should be of the form 'did:btcr:<txref>'");
        }

        // 3) decode txref to find block height, transaction position, and txo index

        config.query = did;
        t2t::Transaction transaction;

        t2t::decodeTxref(btc, config, transaction);

        std::cout << "Valid txref found:\n";
        std::cout << "  txref: " << transaction.txref << "\n";
        std::cout << "  txid: " << transaction.txid << "\n";
        std::cout << "  block height: " << transaction.blockHeight << "\n";
        std::cout << "  transaction position: " << transaction.position << "\n";
        std::cout << "  txoIndex: " << transaction.txoIndex << "\n";


        // 4) Is txo spent or not?

        utxoinfo_t utxoinfo = btc.gettxout(transaction.txid, transactionData.txoIndex);

        UnspentData unspentData;
        unspentData.txid = transaction.txid;
        unspentData.utxoIndex = transactionData.txoIndex;
        unspentData.amountSatoshis = btc2satoshi(utxoinfo.value);
        unspentData.scriptPubKeyHex = utxoinfo.scriptPubKey.hex;

        // So, Is txo unspent?
        // yes: this is the latest version of the DID. From this we can construct the DID Document
        // no : recursively follow transaction chain until txo  with an unspent output is found

        // TODO hmm, if btc.gettxout() returns anything, then it is unspent. If it returns nothing,
        // that means it is spent, but we don't know where. How do we "walk the transaction chain"?
        // Probably need to use something from libbitcoin-explorer, like fetch-history

                
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
