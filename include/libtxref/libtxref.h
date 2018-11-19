
#ifndef TXREF_TXREF_H
#define TXREF_TXREF_H


#include <string>

namespace txref {

    // bech32 "human readable part"s
    static const char BECH32_HRP_MAIN[] = "tx";
    static const char BECH32_HRP_TEST[] = "txtest";

    // magic codes used for chain identification and namespacing
    static const char MAGIC_BTC_MAIN = 0x3;
    static const char MAGIC_BTC_MAIN_EXTENDED = 0x4;
    static const char MAGIC_BTC_TEST = 0x6;
    static const char MAGIC_BTC_TEST_EXTENDED = 0x7;

    // characters used when pretty-printing
    static const char colon = ':';
    static const char hyphen = '-';

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
            const std::string & hrp = BECH32_HRP_MAIN
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
            const std::string & hrp = BECH32_HRP_TEST
    );

    // decodes a bech32 encoded "transaction position reference" (txref) and
    // returns identifying data
    LocationData decode(const std::string & txref);

    namespace limits {

        const int TXREF_STRING_MIN_LENGTH = 18;                    // ex: "tx1rqqqqqqqqmhuqhp"
        const int TXREF_STRING_NO_HRP_MIN_LENGTH = 15;             // ex: "rqqqqqqqqmhuqhp"

        const int TXREF_EXT_STRING_MIN_LENGTH = 21;                // ex: "tx1yqqqqqqqqqqqksvh26"
        const int TXREF_EXT_STRING_NO_HRP_MIN_LENGTH = 18;         // ex: "yqqqqqqqqqqqksvh26"

        const int TXREF_STRING_MIN_LENGTH_TESTNET = 22;            // ex: "txtest1rqqqqqqqqmhuqhp"

        const int TXREF_EXT_STRING_MIN_LENGTH_TESTNET = 25;        // ex: "txtest18jk0uqayzu4xaw4hzl"

    }
}


#endif //TXREF_TXREF_H
