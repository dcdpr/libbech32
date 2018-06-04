//
// Created by Pape, Daniel (DS) on 4/30/18.
//

#ifndef TXREF_TXID2TXREF_H
#define TXREF_TXID2TXREF_H

namespace t2t {

    struct Config {
        std::string rpcuser = "";
        std::string rpcpassword ="";
        std::string rpchost = "127.0.0.1";
        int rpcport = 0;
        std::string query ="";
        bool forceExtended = false;
        unsigned int utxoIndex = 0;
    };

    struct Transaction {
        std::string txid = "";
        std::string txref = "";
        std::string network = "";
        int blockHeight = 0;
        int position = 0;
        unsigned int utxoIndex = 0;
        std::string query = "";
    };

}

#endif //TXREF_TXID2TXREF_H
