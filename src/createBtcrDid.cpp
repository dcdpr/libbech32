#include <iostream>
#include <cstdlib>
#include <memory>
#include "BitcoinRPCFacade.h"
#include "ChainQuery.h"
#include "ChainSoQuery.h"
#include "encodeOpReturnData.h"
#include "satoshis.h"
#include <libtxref/txref.h>
#include <bitcoinapi/bitcoinapi.h>
#include <anyoption/anyoption.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

struct Config {
    std::string rpcuser;
    std::string rpcpassword;
    std::string rpchost = "127.0.0.1";
    int rpcport = 0;
    std::string query;
};

struct TransactionData {
    std::string inputTxid;
    std::string inputAddress;
    std::string outputAddress;
    std::string network;
    std::string wif;
    std::string ddoRef;
    double fee;
};



std::string find_homedir() {
    std::string ret;
    char * home = getenv("HOME");
    if(home != nullptr)
        ret.append(home);
    return ret;
}

int parseCommandLineArgs(int argc, char **argv, struct Config &config, struct TransactionData &transactionData) {

    auto opt = new AnyOption();
    opt->setFileDelimiterChar('=');

    opt->addUsage( "" );
    opt->addUsage( "Usage: txid2txref [options] <inputTxid> <changeAddress> <network> <WIF> <fee> <ddoRef>" );
    opt->addUsage( "" );
    opt->addUsage( " -h  --help                 Print this help " );
    opt->addUsage( " --rpchost [rpchost or IP]  RPC host (default: 127.0.0.1) " );
    opt->addUsage( " --rpcuser [user]           RPC user " );
    opt->addUsage( " --rpcpassword [pass]       RPC password " );
    opt->addUsage( " --rpcport [port]           RPC port (default: try both 8332 and 18332) " );
    opt->addUsage( " --config [config_path]     Full pathname to bitcoin.conf (default: <homedir>/.bitcoin/bitcoin.conf) " );
    opt->addUsage( "" );
    opt->addUsage( "<inputAddress>  input bitcoin address: needs at least slightly more unspent BTCs than your offered fee" );
//    opt->addUsage( "<inputTxid>     input txid: needs at least slightly more unspent BTCs than your offered fee" );
    opt->addUsage( "<outputAddress>  output bitcoin address: will receive transaction change and be the basis for your DID" );
    opt->addUsage( "<network>       the bitcoin network you want to use, 'main' or 'test'" );
    opt->addUsage( "<WIF>           WIF representation of your private key" );
    opt->addUsage( "<fee>           fee you are willing to pay (suggestion: >0.001 BTC)" );
    opt->addUsage( "<ddoRef>        reference to a DDO you want as part of your DID (optional)" );

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

    // get the positional arguments
    if(opt->getArgc() < 5) {
        std::cerr << "Error: all required arguments not found. Check command line usage." << std::endl;
        opt->printUsage();
        delete opt;
        return -1;
    }
    transactionData.inputAddress = opt->getArgv(0);
//    transactionData.inputTxid = opt->getArgv(0);
    transactionData.outputAddress = opt->getArgv(1);
    transactionData.network = opt->getArgv(2);
    transactionData.wif = opt->getArgv(3);
    transactionData.fee = std::atof(opt->getArgv(4));
    if(opt->getArgv(5) != nullptr)
        transactionData.ddoRef = opt->getArgv(5);

    return 1;
}


int main(int argc, char *argv[]) {

    struct Config config;
    struct TransactionData transactionData;

    int ret = parseCommandLineArgs(argc, argv, config, transactionData);
    if (ret < 1) {
        std::exit(ret);
    }

    try {

        BitcoinRPCFacade btc(config.rpcuser, config.rpcpassword, config.rpchost, config.rpcport);

        // TODO validate addresses

        // 1. Get unspent amount available from inputAddress

        UnspentData unspentData;
        utxoinfo_t utxoinfo;

        if(!transactionData.inputAddress.empty()) {
            // get unspent amount and txid from inputAddress
            ChainQuery *q = new ChainSoQuery();
            unspentData =
                    q->getUnspentOutputs(transactionData.inputAddress, 0, transactionData.network);
        }
        else if(!transactionData.inputTxid.empty()) {
            utxoinfo = btc.gettxout(transactionData.inputAddress, 0);
        }

        // 2. compute change needed to go to outputAddress

        int64_t change = unspentData.amountSatoshis - btc2satoshi(transactionData.fee);

        // 3. create DID transaction and submit to network

        // add input txid and tx index
        std::vector<txout_t> inputs;
        txout_t txout = {unspentData.txid, 0};
        inputs.push_back(txout);

        // add output address and the change amount
        std::map<std::string, std::string> amounts;

        // first output is the output address for the change
        amounts.insert(std::make_pair(transactionData.outputAddress, std::to_string(satoshi2btc(change)) ));

        // second output is the OP_RETURN
        std::string encoded_op_return = encodeOpReturnData(transactionData.ddoRef);
        amounts.insert(std::make_pair("data", encoded_op_return));

        std::string rawTransaction = btc.createrawtransaction(inputs, amounts);

        // sign with private key
        signrawtxin_t signrawtxin;
        signrawtxin.txid = unspentData.txid;
        signrawtxin.n = static_cast<unsigned int>(unspentData.index);
        signrawtxin.scriptPubKey = unspentData.scriptPubKeyHex;
        signrawtxin.redeemScript = "";

        std::string signedRawTransaction =
                btc.signrawtransaction(rawTransaction, {signrawtxin}, {transactionData.wif}, "ALL");

        // broadcast to network
        std:string resultTxid = btc.sendrawtransaction(signedRawTransaction);

        if(!resultTxid.empty())
            std::cout << resultTxid << std::endl;
        else
            std::cout << "the network did not accept our transaction" << std::endl;
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
