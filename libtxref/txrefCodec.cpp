
#include "txrefCodec.h"
#include "bech32.h"
#include <vector>
#include <stdexcept>
#include <sstream>

namespace {

    using namespace txref::limits;

    const int MAX_BLOCK_HEIGHT         = 0xFFFFFF; // 16777215

    const int MAX_TRANSACTION_POSITION = 0x7FFF;   // 32767

    const int MAX_TXO_INDEX            = 0x7FFF;   // 32767

    const int MAX_MAGIC_CODE           = 0x1F;

    const int DATA_SIZE                = 9;

    const int DATA_EXTENDED_SIZE       = 13;


    bool isStandardSize(unsigned long dataSize) {
        return dataSize == DATA_SIZE;
    }

    bool isExtendedSize(unsigned long dataSize) {
        return dataSize == DATA_EXTENDED_SIZE;
    }

    bool isDataSizeValid(unsigned long dataSize) {
        return isStandardSize(dataSize) || isExtendedSize(dataSize);
    }

    // is a txref string, missing the HRP, still of a valid length for a txref?
    bool isLengthValid(unsigned long length) {
        return length == TXREF_STRING_NO_HRP_MIN_LENGTH ||
               length == TXREF_EXT_STRING_NO_HRP_MIN_LENGTH;
    }

    // a block's height can only be in a certain range
    void checkBlockHeightRange(int blockHeight) {
        if(blockHeight < 0 || blockHeight > MAX_BLOCK_HEIGHT)
            throw std::runtime_error("block height is too large");
    }

    // a transaction's position can only be in a certain range
    void checkTransactionPositionRange(int transactionPosition) {
        if(transactionPosition < 0 || transactionPosition > MAX_TRANSACTION_POSITION)
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

    // extract the magic code from the decoded data part
    void extractMagicCode(uint8_t & magicCode, const bech32::HrpAndDp &hd) {
        magicCode = hd.dp[0];
    }

    // extract the version from the decoded data part
    void extractVersion(uint8_t & version, const bech32::HrpAndDp &hd) {
        version = hd.dp[1] & 0x1u;
    }

    // extract the extended version from the decoded data part
    void extractExtendedVersion(uint8_t & extendedVersion, const bech32::HrpAndDp &hd) {
        uint8_t version = 0;
        extractVersion(version, hd);
        if(version != 1)
            throw std::runtime_error("version==0, so there can be no extended version");
        extendedVersion = (hd.dp[1] >> 1u);
        extendedVersion |= ((hd.dp[2] & 0x1u) << 4u);
    }

    // extract the block height from the decoded data part
    void extractBlockHeight(int & blockHeight, const bech32::HrpAndDp &hd) {
        uint8_t version = 0;
        uint8_t extendedVersion = 0;
        extractVersion(version, hd);
        if(version == 1)
            extractExtendedVersion(extendedVersion, hd);

        if(version == 0) {
            blockHeight = (hd.dp[1] >> 1u);
            blockHeight |= (hd.dp[2] << 4u);
            blockHeight |= (hd.dp[3] << 9u);
            blockHeight |= (hd.dp[4] << 14u);
            blockHeight |= (hd.dp[5] << 19u);
        }
        else if(version == 1 && extendedVersion == 0) {
            blockHeight = (hd.dp[2] >> 1u);
            blockHeight |= (hd.dp[3] << 4u);
            blockHeight |= (hd.dp[4] << 9u);
            blockHeight |= (hd.dp[5] << 14u);
            blockHeight |= (hd.dp[6] << 19u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref extended version detected: " << static_cast<int>(extendedVersion);
            throw std::runtime_error(ss.str());
        }
    }

    // extract the transaction position from the decoded data part
    void extractTransactionPosition(int & transactionPosition, const bech32::HrpAndDp &hd) {
        uint8_t version = 0;
        uint8_t extendedVersion = 0;
        extractVersion(version, hd);
        if(version == 1)
            extractExtendedVersion(extendedVersion, hd);

        if(version == 0) {
            transactionPosition = hd.dp[6];
            transactionPosition |= (hd.dp[7] << 5u);
            transactionPosition |= (hd.dp[8] << 10u);
        }
        else if(version == 1 && extendedVersion == 0) {
            transactionPosition = hd.dp[7];
            transactionPosition |= (hd.dp[8] << 5u);
            transactionPosition |= (hd.dp[9] << 10u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref extended version detected: " << static_cast<int>(extendedVersion);
            throw std::runtime_error(ss.str());
        }
    }

    // extract the TXO index from the decoded data part
    void extractTxoIndex(int &txoIndex, const bech32::HrpAndDp &hd) {
        uint8_t version = 0;
        uint8_t extendedVersion = 0;
        extractVersion(version, hd);
        if(version == 1)
            extractExtendedVersion(extendedVersion, hd);

        if(version == 0) {
            txoIndex = 0;
        }
        else if(version == 1 and extendedVersion == 0) {
            txoIndex = hd.dp[10];
            txoIndex |= (hd.dp[11] << 5u);
            txoIndex |= (hd.dp[12] << 10u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref extended version detected: " << static_cast<int>(extendedVersion);
            throw std::runtime_error(ss.str());
        }
    }

    // some txref strings may have had the HRP stripped off. Attempt to prepend one if needed.
    // assumes that bech32::stripUnknownChars() has already been called
    std::string addHrpIfNeeded(const std::string & txref) {
        if(isLengthValid(txref.length()) && txref.at(0) == 'r') {
            return std::string(txref::BECH32_HRP_MAIN) + bech32::separator + txref;
        }
        if(isLengthValid(txref.length()) && txref.at(0) == 'x') {
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
        dp[5] |= (bh & 0xF80000u) >> 19u;         // sets 5 bits in 6th 5 bits (24 bits total for blockHeight)

        // set transaction position
        dp[6] |= (tp & 0x1Fu);                    // sets 5 bits in 7th 5 bits
        dp[7] |= (tp & 0x3E0u) >> 5u;             // sets 5 bits in 8th 5 bits
        dp[8] |= (tp & 0x7C00u) >> 10u;           // sets 5 bits in 9th 5 bits (15 bits total for transactionPosition)

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

        // set version bit to 1
        uint8_t version = 1;
        dp[1] = version;                          // sets 1 bit in 2nd 5 bits

        // set extended version
        uint8_t extendedVersion = 0;
        dp[1] |= (extendedVersion & 0xFu) << 1u;  // sets 4 bits in 2nd 5 bits
        dp[2] |= (extendedVersion & 0x10u) >> 4u; // sets 1 bit in 3rd 5 bits (5 bits total for extended version)

        // set block height
        dp[2] |= (bh & 0xFu) << 1u;               // sets 4 bits in 3rd 5 bits
        dp[3] |= (bh & 0x1F0u) >> 4u;             // sets 5 bits in 4th 5 bits
        dp[4] |= (bh & 0x3E00u) >> 9u;            // sets 5 bits in 5th 5 bits
        dp[5] |= (bh & 0x7C000u) >> 14u;          // sets 5 bits in 6th 5 bits
        dp[6] |= (bh & 0xF80000u) >> 19u;         // sets 5 bits in 7th 5 bits (24 bits total for blockHeight)

        // set transaction position
        dp[7] |= (tp & 0x1Fu);                    // sets 5 bits in 8th 5 bits
        dp[8] |= (tp & 0x3E0u) >> 5u;             // sets 5 bits in 9th 5 bits
        dp[9] |= (tp & 0x7C00u) >> 10u;           // sets 5 bits in 10th 5 bits (15 bits total for transactionPosition)

        // set txo index
        dp[10] |= ti & 0x1Fu;                     // sets 5 bits in 11th 5 bits
        dp[11] |= (ti & 0x3E0u) >> 5u;            // sets 5 bits in 12th 5 bits
        dp[12] |= (ti & 0x7C00u) >> 10u;          // sets 5 bits in 13th 5 bits (15 bits total for txoIndex)

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
            return txrefEncode(hrp, magicCode, blockHeight, transactionPosition);

        return txrefExtEncode(hrp, magicCode, blockHeight, transactionPosition, txoIndex);

    }

    LocationData decode(const std::string & txref) {

        std::string txrefClean = bech32::stripUnknownChars(txref);
        txrefClean = addHrpIfNeeded(txrefClean);
        bech32::HrpAndDp bs = bech32::decode(txrefClean);

        auto dataSize = bs.dp.size();
        if(!isDataSizeValid(dataSize)) {
            throw std::runtime_error("decoded dp size is incorrect");
        }

        uint8_t magicCode;
        extractMagicCode(magicCode, bs);

        LocationData data;
        data.txref = addDashes(txrefClean, bs.hrp.length());
        data.hrp = bs.hrp;
        data.magicCode = magicCode;
        extractBlockHeight(data.blockHeight, bs);
        extractTransactionPosition(data.transactionPosition, bs);
        extractTxoIndex(data.txoIndex, bs);

        return data;
    }

}
