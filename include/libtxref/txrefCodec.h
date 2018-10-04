
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
    // an extended reference is returned (txref-ext). If txoIndex is zero,
    // but forceExtended=true, then an extended reference is returned
    // (txref-ext).
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
    // an extended reference is returned (txref-ext). If txoIndex is zero,
    // but forceExtended=true, then an extended reference is returned
    // (txref-ext).
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

        const int TXREF_STRING_MIN_LENGTH = 18;                    // ex: "tx1rqqqqqqqqmhuqhp"
        const int TXREF_STRING_NO_HRP_MIN_LENGTH = 15;             // ex: "rqqqqqqqqmhuqhp"

        const int TXREF_EXT_STRING_MIN_LENGTH = 22;                // ex: "tx1rpqqqqqqqqqqq2geahz"
        const int TXREF_EXT_STRING_NO_HRP_MIN_LENGTH = 19;         // ex: "rpqqqqqqqqqqq2geahz"

        const int TXREF_STRING_MIN_LENGTH_TESTNET = 22;            // ex: "txtest1rqqqqqqqqmhuqhp"

        const int TXREF_EXT_STRING_MIN_LENGTH_TESTNET = 26;        // ex: "txtest1xpjk0uqayzu4xgrlpue"

    }
}


#endif //TXREF_TXREF_H
