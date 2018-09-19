
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
        std::string txref;
        int blockHeight = 0;
        int transactionPosition = 0;
        int txoIndex = 0;
        int magicCode = 0;
    };

    // encodes the position of a confirmed bitcoin transaction on the
    // mainnet network and returns a bech32 encoded "transaction
    // position reference" (txref). If txoIndex is greater than 0, then
    // an extended reference is returned (txref-ext)
    std::string encode(
            int blockHeight,
            int transactionPosition,
            int txoIndex = 0,
            bool forceExtended = false,
            const std::string & hrp = BECH32_HRP_MAIN,
            int magicCode = MAGIC_BTC_MAIN
    );

    // encodes the position of a confirmed bitcoin transaction on the
    // testnet network and returns a bech32 encoded "transaction
    // position reference" (txref). If txoIndex is greater than 0, then
    // an extended reference is returned (txref-ext)
    std::string encodeTestnet(
            int blockHeight,
            int transactionPosition,
            int txoIndex = 0,
            bool forceExtended = false,
            const std::string & hrp = BECH32_HRP_TEST,
            int magicCode = MAGIC_BTC_TEST
    );

    // decodes a bech32 encoded "transaction position reference" (txref) and
    // returns identifying data
    LocationData decode(const std::string & txref);

    namespace limits {

        const int TXREF_STRING_MIN_LENGTH = 17;                    // ex: "tx1rqqqqqqqqmhuqk"
        const int TXREF_STRING_NO_HRP_MIN_LENGTH = 14;             // ex: "rqqqqqqqqmhuqk"

        const int TXREF_EXT_STRING_MIN_LENGTH = 20;                // ex: "tx1rqqqqqqqqqquau7hl"
        const int TXREF_EXT_STRING_NO_HRP_MIN_LENGTH = 17;         // ex: "rqqqqqqqqqquau7hl"

        const int TXREF_STRING_MIN_LENGTH_TESTNET = 23;            // ex: "txtest1xqqqqqqqqqkn3gh9"
        const int TXREF_STRING_NO_HRP_MIN_LENGTH_TESTNET = 16;     // ex: "xqqqqqqqqqkn3gh9"

        const int TXREF_EXT_STRING_MIN_LENGTH_TESTNET = 26;        // ex: "txtest1xqqqqqqqqqqqqj7dvzy"
        const int TXREF_EXT_STRING_NO_HRP_MIN_LENGTH_TESTNET = 19; // ex: "xqqqqqqqqqqqqj7dvzy"

    }
}


#endif //TXREF_TXREF_H
