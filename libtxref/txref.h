
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
        char magicCode = 0;
        int blockHeight = 0;
        int transactionPosition = 0;
        int uxtoIndex = 0;
        std::string txref;
    };

    // encodes the position of a confirmed bitcoin transaction on the
    // mainnet network and returns a bech32 encoded "transaction
    // position reference" (txref). If utxoIndex is greater than 0, then
    // an extended reference is returned (txref-ext)
    std::string encode(
            int blockHeight,
            int transactionPosition,
            int utxoIndex = 0,
            bool forceExtended = false,
            const std::string & hrp = BECH32_HRP_MAIN,
            char magicCode = MAGIC_BTC_MAIN
    );

    // encodes the position of a confirmed bitcoin transaction on the
    // testnet network and returns a bech32 encoded "transaction
    // position reference" (txref). If utxoIndex is greater than 0, then
    // an extended reference is returned (txref-ext)
    std::string encodeTestnet(
            int blockHeight,
            int transactionPosition,
            int utxoIndex = 0,
            bool forceExtended = false,
            const std::string & hrp = BECH32_HRP_TEST,
            char magicCode = MAGIC_BTC_TEST
    );

    // decodes a bech32 encoded "transaction position reference" (txref) and
    // returns identifying data
    LocationData decode(const std::string & txref);

}


#endif //TXREF_TXREF_H
