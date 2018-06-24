
#include "txref.h"
#include "../libbech32/bech32.h"
#include <vector>
#include <stdexcept>

namespace {

    using namespace txref::limits;

    const int MAX_BLOCK_HEIGHT         = 0x1FFFFF;
    const int MAX_BLOCK_HEIGHT_TESTNET = 0x3FFFFFF;

    const int MAX_TRANSACTION_POSITION         = 0x1FFF;
    const int MAX_TRANSACTION_POSITION_TESTNET = 0x3FFFF;

    const int MAX_TXO_INDEX           = 0x1FFF;

    const int MAX_MAGIC_CODE = 0x1F;

    const int DATA_SIZE = 8;
    const int DATA_SIZE_TESTNET = 10;

    const int DATA_EXTENDED_SIZE = 11;
    const int DATA_EXTENDED_SIZE_TESTNET = 13;


    bool isStandardSizeForMainNet(unsigned long dataSize) {
        return dataSize == DATA_SIZE;
    }

    bool isStandardSizeForTestNet(unsigned long dataSize) {
        return dataSize == DATA_SIZE_TESTNET;
    }

    bool isExtendedSizeForMainNet(unsigned long dataSize) {
        return dataSize == DATA_EXTENDED_SIZE;
    }

    bool isExtendedSizeForTestNet(unsigned long dataSize) {
        return dataSize == DATA_EXTENDED_SIZE_TESTNET;
    }

    bool isDataSizeValidForMainNet(unsigned long dataSize) {
        return isStandardSizeForMainNet(dataSize) ||
               isExtendedSizeForMainNet(dataSize);
    }

    bool isDataSizeValidForTestNet(unsigned long dataSize) {
        return isStandardSizeForTestNet(dataSize) ||
               isExtendedSizeForTestNet(dataSize);
    }

    bool isDataSizeValid(unsigned long dataSize) {
        return isStandardSizeForMainNet(dataSize) ||
               isStandardSizeForTestNet(dataSize) ||
               isExtendedSizeForMainNet(dataSize) ||
               isExtendedSizeForTestNet(dataSize);
    }

    // is a txref string, missing the HRP, still of a valid length for a mainnet txref?
    bool isLengthValidForMainNet(unsigned long length) {
        return length == TXREF_STRING_NO_HRP_MIN_LENGTH ||
                length == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH;
    }

    // is a txref string, missing the HRP, still of a valid length for a testnet txref?
    bool isLengthValidForTestNet(unsigned long length) {
        return length == TXREF_STRING_NO_HRP_MIN_LENGTH_TESTNET ||
                length == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH_TESTNET;
    }

    // a block's height can only be in a certain range for mainnet
    void checkBlockHeightRange(int blockHeight) {
        if(blockHeight < 0 || blockHeight > MAX_BLOCK_HEIGHT)
            throw std::runtime_error("block height is too large");
    }

    // a block's height can only be in a certain range for testnet
    void checkBlockHeightRangeTestnet(int blockHeight) {
        if(blockHeight < 0 || blockHeight > MAX_BLOCK_HEIGHT_TESTNET)
            throw std::runtime_error("block height is too large");
    }

    // a transaction's position can only be in a certain range for mainnet
    void checkTransactionPositionRange(int transactionPosition) {
        if(transactionPosition < 0 || transactionPosition > MAX_TRANSACTION_POSITION)
            throw std::runtime_error("transaction position is too large");
    }

    // a transaction's position can only be in a certain range for testnet
    void checkTransactionPositionRangeTestnet(int transactionPosition) {
        if(transactionPosition < 0 || transactionPosition > MAX_TRANSACTION_POSITION_TESTNET)
            throw std::runtime_error("transaction position is too large");
    }

    // a TXO's index can only be in a certain range
    void checkTxoIndexRange(int txoIndex) {
        if(txoIndex < 0 || txoIndex > MAX_TXO_INDEX)
            throw std::runtime_error("txo index is too large");
    }

    // the magic code can only be in a certain range
    void checkMagicCodeRange(int magicCode) {
        if(magicCode < 0 || magicCode > MAX_MAGIC_CODE)
            throw std::runtime_error("magic code is too large");
    }

    // add dashes to the txref string to make it look nicer
    std::string addDashes(const std::string & raw, std::string::size_type hrplen) {
        if(hrplen > bech32::limits::MAX_HRP_LENGTH)
            throw std::runtime_error("HRP must be less than 84 characters long");

        auto hrpLength = static_cast<std::string::difference_type>(hrplen);

        std::string output(raw);
        output.insert(output.begin()+hrpLength+1, '-');
        output.insert(output.begin()+hrpLength+6, '-');
        output.insert(output.begin()+hrpLength+11, '-');
        output.insert(output.begin()+hrpLength+16, '-');
        return output;
    }

    // extract the block height from the decoded data part
    void extractBlockHeight(int & blockHeight, const bech32::HrpAndDp &hd) {
        blockHeight = (hd.dp[1] >> 1u);
        blockHeight |= (hd.dp[2] << 4u);
        blockHeight |= (hd.dp[3] << 9u);
        blockHeight |= (hd.dp[4] << 14u);
        auto dataSize = hd.dp.size();
        if (isDataSizeValidForMainNet(dataSize)) {
            blockHeight |= ((hd.dp[5] & 0x03u) << 19u);
        }
        else if (isDataSizeValidForTestNet(dataSize)) {
            blockHeight |= (hd.dp[5] << 19u);
            blockHeight |= ((hd.dp[6] & 0x03u) << 24u);
        }
    }

    // extract the transaction position from the decoded data part
    void extractTransactionPosition(int & transactionPosition, const bech32::HrpAndDp &hd) {
        auto dataSize = hd.dp.size();
        if (isDataSizeValidForMainNet(dataSize)) {
            transactionPosition = ((hd.dp[5] & 0x1Cu) >> 2u);
            transactionPosition |= (hd.dp[6] << 3u);
            transactionPosition |= (hd.dp[7] << 8u);
        }
        else if (isDataSizeValidForTestNet(dataSize)) {
            transactionPosition = ((hd.dp[6] & 0x1Cu) >> 2u);
            transactionPosition |= (hd.dp[7] << 3u);
            transactionPosition |= (hd.dp[8] << 8u);
            transactionPosition |= (hd.dp[9] << 13u);
        }
    }

    // extract the TXO index from the decoded data part
    void extractTxoIndex(int &txoIndex, const bech32::HrpAndDp &hd) {
        auto dataSize = hd.dp.size();
        if (isExtendedSizeForMainNet(dataSize)) {
            txoIndex = hd.dp[8];
            txoIndex |= (hd.dp[9] << 5u);
            txoIndex |= (hd.dp[10] << 10u);
        }
        else if (isExtendedSizeForTestNet(dataSize)) {
            txoIndex = hd.dp[10];
            txoIndex |= (hd.dp[11] << 5u);
            txoIndex |= (hd.dp[12] << 10u);
        }
    }

    // some txref strings may have had the HRP stripped off. Attempt to prepend one if needed.
    // assumes that bech32::stripUnknownChars() has already been called
    std::string addHrpIfNeeded(const std::string & txref) {
        if(isLengthValidForMainNet(txref.length()) && txref.at(0) == 'r') {
            return std::string(txref::BECH32_HRP_MAIN) + bech32::separator + txref;
        }
        if(isLengthValidForTestNet(txref.length()) && txref.at(0) == 'x') {
            return std::string(txref::BECH32_HRP_TEST) + bech32::separator + txref;
        }
        return txref;
    }


    std::string txrefEncode(
            const std::string &hrp,
            int magicCode,
            int blockHeight,
            int transactionPosition) {

        checkBlockHeightRange(blockHeight);
        checkTransactionPositionRange(transactionPosition);
        checkMagicCodeRange(magicCode);

        // ranges have been checked. make unsigned copies of params
        auto bh = static_cast<uint32_t>(blockHeight);
        auto tp = static_cast<uint32_t>(transactionPosition);

        std::vector<unsigned char> dp(DATA_SIZE);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);  // sets 1-3 bits in the 1st 5 bits

        // set version bit to 0
        dp[1] &= ~(1u << 0u);                     // sets 1 bit in 2nd 5 bits

        // set block height
        dp[1] |= (bh & 0xFu) << 1u;               // sets 4 bits in 2nd 5 bits
        dp[2] |= (bh & 0x1F0u) >> 4u;             // sets 5 bits in 3rd 5 bits
        dp[3] |= (bh & 0x3E00u) >> 9u;            // sets 5 bits in 4th 5 bits
        dp[4] |= (bh & 0x7C000u) >> 14u;          // sets 5 bits in 5th 5 bits
        dp[5] |= (bh & 0x180000u) >> 19u;         // sets 2 bits in 6th 5 bits (21 bits total for blockHeight)

        // set transaction position
        dp[5] |= (tp & 0x7u) << 2u;               // sets 3 bits in 6th 5 bits
        dp[6] |= (tp & 0xF8u) >> 3u;              // sets 5 bits in 7th 5 bits
        dp[7] |= (tp & 0x1F00u) >> 8u;            // sets 5 bits in 8th 5 bits (13 bits total for transactionPosition)

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = addDashes(result, hrp.length());

        return output;
    }

    std::string txrefExtEncode(
            const std::string &hrp,
            int magicCode,
            int blockHeight,
            int transactionPosition,
            int txoIndex) {

        checkBlockHeightRange(blockHeight);
        checkTransactionPositionRange(transactionPosition);
        checkTxoIndexRange(txoIndex);
        checkMagicCodeRange(magicCode);

        // ranges have been checked. make unsigned copies of params
        auto bh = static_cast<uint32_t>(blockHeight);
        auto tp = static_cast<uint32_t>(transactionPosition);
        auto ti = static_cast<uint32_t>(txoIndex);

        std::vector<unsigned char> dp(DATA_EXTENDED_SIZE);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);  // sets 1-3 bits in the 1st 5 bits

        // set version bit to 0 (clear bit)
        dp[1] &= ~(1u << 0u);                     // sets 1 bit in 2nd 5 bits

        // set block height
        dp[1] |= (bh & 0xFu) << 1u;               // sets 4 bits in 2nd 5 bits
        dp[2] |= (bh & 0x1F0u) >> 4u;             // sets 5 bits in 3rd 5 bits
        dp[3] |= (bh & 0x3E00u) >> 9u;            // sets 5 bits in 4th 5 bits
        dp[4] |= (bh & 0x7C000u) >> 14u;          // sets 5 bits in 5th 5 bits
        dp[5] |= (bh & 0x180000u) >> 19u;         // sets 2 bits in 6th 5 bits (21 bits total for blockHeight)

        // set transaction position
        dp[5] |= (tp & 0x7u) << 2u;               // sets 3 bits in 6th 5 bits
        dp[6] |= (tp & 0xF8u) >> 3u;              // sets 5 bits in 7th 5 bits
        dp[7] |= (tp & 0x1F00u) >> 8u;            // sets 5 bits in 8th 5 bits (13 bits total for transactionPosition)

        // set txo index
        dp[8] |= ti & 0x1Fu;                      // sets 5 bits in 9th 5 bits
        dp[9] |= (ti & 0x3E0u) >> 5u;             // sets 5 bits in 10th 5 bits
        dp[10] |= (ti & 0x1C00u) >> 10u;          // sets 3 bits in 11th 5 bits (13 bits total for txoIndex)

        // clear last two bits for now
        dp[10] &= ~(1u << 4u);                    // two bits leftover for future expansion
        dp[10] &= ~(1u << 5u);

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = addDashes(result, hrp.length());

        return output;
    }

    std::string txrefEncodeTestnet(
            const std::string &hrp,
            int magicCode,
            int blockHeight,
            int transactionPosition) {

        checkBlockHeightRangeTestnet(blockHeight);
        checkTransactionPositionRangeTestnet(transactionPosition);
        checkMagicCodeRange(magicCode);

        // ranges have been checked. make unsigned copies of params
        auto bh = static_cast<uint32_t>(blockHeight);
        auto tp = static_cast<uint32_t>(transactionPosition);

        std::vector<unsigned char> dp(DATA_SIZE_TESTNET);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);  // sets 1-3 bits in the 1st 5 bits

        // set version bit to 0
        dp[1] &= ~(1u << 0u);                     // sets 1 bit in 2nd 5 bits

        // set block height
        dp[1] |= (bh & 0xFu) << 1u;               // sets 4 bits in 2nd 5 bits
        dp[2] |= (bh & 0x1F0u) >> 4u;             // sets 5 bits in 3rd 5 bits
        dp[3] |= (bh & 0x3E00u) >> 9u;            // sets 5 bits in 4th 5 bits
        dp[4] |= (bh & 0x7C000u) >> 14u;          // sets 5 bits in 5th 5 bits
        dp[5] |= (bh & 0xF80000u) >> 19u;         // sets 5 bits in 6th 5 bits
        dp[6] |= (bh & 0x3000000u) >> 24u;        // sets 2 bits in 7th 5 bits (26 bits total for blockHeight)

        // set transaction position
        dp[6] |= (tp & 0x7u) << 2u;               // sets 3 bits in 8th 5 bits
        dp[7] |= (tp & 0xF8u) >> 3u;              // sets 5 bits in 9th 5 bits
        dp[8] |= (tp & 0x1F00u) >> 8u;            // sets 5 bits in 10th 5 bits
        dp[9] |= (tp & 0x3E000u) >> 13u;          // sets 5 bits in 11th 5 bits (18 bits total for transactionPosition)

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = addDashes(result, hrp.length());

        return output;
    }

    std::string txrefExtEncodeTestnet(
            const std::string &hrp,
            int magicCode,
            int blockHeight,
            int transactionPosition,
            int txoIndex) {

        checkBlockHeightRangeTestnet(blockHeight);
        checkTransactionPositionRangeTestnet(transactionPosition);
        checkTxoIndexRange(txoIndex);
        checkMagicCodeRange(magicCode);

        // ranges have been checked. make unsigned copies of params
        auto bh = static_cast<uint32_t>(blockHeight);
        auto tp = static_cast<uint32_t>(transactionPosition);
        auto ti = static_cast<uint32_t>(txoIndex);

        std::vector<unsigned char> dp(DATA_EXTENDED_SIZE_TESTNET);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);  // sets 1-3 bits in the 1st 5 bits

        // set version bit to 0
        dp[1] &= ~(1u << 0u);                     // sets 1 bit in 2nd 5 bits

        // set block height
        dp[1] |= (bh & 0xFu) << 1u;               // sets 4 bits in 2nd 5 bits
        dp[2] |= (bh & 0x1F0u) >> 4u;             // sets 5 bits in 3rd 5 bits
        dp[3] |= (bh & 0x3E00u) >> 9u;            // sets 5 bits in 4th 5 bits
        dp[4] |= (bh & 0x7C000u) >> 14u;          // sets 5 bits in 5th 5 bits
        dp[5] |= (bh & 0xF80000u) >> 19u;         // sets 5 bits in 6th 5 bits
        dp[6] |= (bh & 0x3000000u) >> 24u;        // sets 2 bits in 7th 5 bits (26 bit total for blockHeight)

        // set transaction position
        dp[6] |= (tp & 0x7u) << 2u;               // sets 3 bits in 7th 5 bits
        dp[7] |= (tp & 0xF8u) >> 3u;              // sets 5 bits in 8th 5 bits
        dp[8] |= (tp & 0x1F00u) >> 8u;            // sets 5 bits in 9th 5 bits
        dp[9] |= (tp & 0x3E000u) >> 13u;          // sets 5 bits in 10th 5 bits (18 bits total for transactionPosition)

        // set txo index
        dp[10] |= ti & 0x1Fu;                     // sets 5 bits in 11th 5 bits
        dp[11] |= (ti & 0x3E0u) >> 5u;            // sets 5 bits in 12th 5 bits
        dp[12] |= (ti & 0x1C00u) >> 10u;          // sets 3 bits in 13th 5 bits

        // clear last two bits for now
        dp[12] &= ~(1u << 4u);                    // two bits leftover for future expansion
        dp[12] &= ~(1u << 5u);

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = addDashes(result, hrp.length());

        return output;
    }


}

namespace txref {

    std::string encode(
            int blockHeight,
            int transactionPosition,
            int txoIndex,
            bool forceExtended,
            const std::string & hrp,
            int magicCode) {

        if(txoIndex == 0 && !forceExtended)
            return txrefEncode(hrp, magicCode, blockHeight, transactionPosition);

        return txrefExtEncode(hrp, magicCode, blockHeight, transactionPosition, txoIndex);

    }

    std::string encodeTestnet(
            int blockHeight,
            int transactionPosition,
            int txoIndex,
            bool forceExtended,
            const std::string & hrp,
            int magicCode) {

        if(txoIndex == 0 && !forceExtended)
            return txrefEncodeTestnet(hrp, magicCode, blockHeight, transactionPosition);

        return txrefExtEncodeTestnet(hrp, magicCode, blockHeight, transactionPosition, txoIndex);

    }

    LocationData decode(const std::string & txref) {

        std::string txrefClean = bech32::stripUnknownChars(txref);
        txrefClean = addHrpIfNeeded(txrefClean);
        bech32::HrpAndDp bs = bech32::decode(txrefClean);

        auto dataSize = bs.dp.size();
        if(!isDataSizeValid(dataSize)) {
            throw std::runtime_error("decoded dp size is incorrect");
        }

        LocationData data;
        data.txref = addDashes(txrefClean, bs.hrp.length());
        data.hrp = bs.hrp;
        data.magicCode = bs.dp[0];
        extractBlockHeight(data.blockHeight, bs);
        extractTransactionPosition(data.transactionPosition, bs);
        extractTxoIndex(data.txoIndex, bs);

        return data;
    }

}
