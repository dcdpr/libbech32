
#ifndef TXREF_TXREF_H
#define TXREF_TXREF_H


#include <string>

namespace txref {

    static const char BECH32_HRP_MAIN[] = "tx";
    static const char BECH32_HRP_TEST[] = "txtest";

    static const char MAGIC_BTC_MAIN = 0x3;
    static const char MAGIC_BTC_TEST = 0x6;

    struct LocationData {
        std::string hrp;
        char magicCode;
        int blockHeight;
        int transactionPosition;
        std::string txref;
    };

    // encodes the position of a confirmed bitcoin transaction on the
    // mainnet network and returns a bech32 encoded "transaction
    // position reference" (txref)
    std::string bitcoinTxrefEncode(
            const std::string & hrp,
            char magicCode,
            int blockHeight,
            int transactionPosition
    );

    // encodes the position of a confirmed bitcoin transaction on the
    // testnet network and returns a bech32 encoded "transaction
    // position reference" (txref)
    std::string bitcoinTxrefEncodeTestnet(
            const std::string & hrp,
            char magicCode,
            int blockHeight,
            int transactionPosition
    );

    // decodes a bech32 encoded "transaction position reference" (txref) and
    // returns identifying data
    LocationData bitcoinTxrefDecode(const std::string & txref);

}


#endif //TXREF_TXREF_H
