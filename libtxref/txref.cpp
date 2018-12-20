
#include "libtxref.h"
#include "libbech32.h"
#include <algorithm>
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

    const int DATA_EXTENDED_SIZE       = 12;


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

    // check that the magic code is for one of the extended txrefs
    void checkExtendedMagicCode(int magicCode) {
        if(magicCode != txref::MAGIC_BTC_MAIN_EXTENDED && magicCode != txref::MAGIC_BTC_TEST_EXTENDED)
            throw std::runtime_error("magic code does not support extended txrefs");
    }

    // separate groups of chars in the txref string to make it look nicer
    std::string addGroupSeparators(
            const std::string & raw,
            std::string::size_type hrplen,
            std::string::difference_type separatorOffset = 4) {

        if(hrplen > bech32::limits::MAX_HRP_LENGTH)
            throw std::runtime_error("HRP must be less than 84 characters long");

        if(separatorOffset < 1)
            throw std::runtime_error("separatorOffset must be > 0");

        auto rawLength = static_cast<std::string::difference_type>(raw.length());
        auto hrpLength = static_cast<std::string::difference_type>(hrplen);

        if(rawLength < 2)
            throw std::runtime_error("Can't add separator characters to strings with length < 2");

        if(rawLength == hrpLength) // no separators needed
            return raw;

        if(rawLength < hrpLength)
            throw std::runtime_error("HRP length can't be greater than input length");

        // number of separators that will be inserted
        auto numSeparators = (rawLength - hrpLength - 1) / separatorOffset;

        // output length
        auto outputLength = static_cast<std::string::size_type>(rawLength + numSeparators);

        // create output string, starting with all hyphens
        std::string output(outputLength, txref::hyphen);

        // copy over the raw string, skipping every offset # chars, after the HRP
        std::string::difference_type rawPos = 0;
        std::string::size_type outputPos = 0;
        for(const auto &c : raw) {

            output[outputPos++] = c;

            ++rawPos;
            if(rawPos > hrpLength && (rawPos - hrpLength) % separatorOffset == 0)
                ++outputPos;
        }

        return output;
    }

    // pretty print a txref returned by bech32::encode()
    std::string prettyPrint(
            const std::string & plain,
            std::string::size_type hrplen) {

        std::string result = plain;

        // add colon after the HRP and bech32 separator character
        auto hrpPlusSeparatorLength =
                static_cast<std::string::difference_type>(hrplen + 1);
        result.insert(result.cbegin()+hrpPlusSeparatorLength, txref::colon);

        // now add hyphens every 4th character after that
        auto hrpPlusSeparatorAndColonLength = hrplen + 2;
        result = addGroupSeparators(result, hrpPlusSeparatorAndColonLength);

        return result;
    }

            // extract the magic code from the decoded data part
    void extractMagicCode(uint8_t & magicCode, const bech32::HrpAndDp &hd) {
        magicCode = hd.dp[0];
    }

    // extract the version from the decoded data part
    void extractVersion(uint8_t & version, const bech32::HrpAndDp &hd) {
        version = hd.dp[1] & 0x1u;
    }

    // extract the block height from the decoded data part
    void extractBlockHeight(int & blockHeight, const bech32::HrpAndDp &hd) {
        uint8_t version = 0;
        extractVersion(version, hd);

        if(version == 0) {
            blockHeight = (hd.dp[1] >> 1u);
            blockHeight |= (hd.dp[2] << 4u);
            blockHeight |= (hd.dp[3] << 9u);
            blockHeight |= (hd.dp[4] << 14u);
            blockHeight |= (hd.dp[5] << 19u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref version detected: " << static_cast<int>(version);
            throw std::runtime_error(ss.str());
        }
    }

    // extract the transaction position from the decoded data part
    void extractTransactionPosition(int & transactionPosition, const bech32::HrpAndDp &hd) {
        uint8_t version = 0;
        extractVersion(version, hd);

        if(version == 0) {
            transactionPosition = hd.dp[6];
            transactionPosition |= (hd.dp[7] << 5u);
            transactionPosition |= (hd.dp[8] << 10u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref version detected: " << static_cast<int>(version);
            throw std::runtime_error(ss.str());
        }
    }

    // extract the TXO index from the decoded data part
    void extractTxoIndex(int &txoIndex, const bech32::HrpAndDp &hd) {
        if(hd.dp.size() < 12) {
            // non-extended txrefs don't store the txoIndex, so just return 0
            txoIndex = 0;
            return;
        }

        uint8_t version = 0;
        extractVersion(version, hd);

        if(version == 0) {
            txoIndex = hd.dp[9];
            txoIndex |= (hd.dp[10] << 5u);
            txoIndex |= (hd.dp[11] << 10u);
        }
        else {
            std::stringstream ss;
            ss << "Unknown txref version detected: " << static_cast<int>(version);
            throw std::runtime_error(ss.str());
        }
    }

    // some txref strings may have had the HRP stripped off. Attempt to prepend one if needed.
    // assumes that bech32::stripUnknownChars() has already been called
    std::string addHrpIfNeeded(const std::string & txref) {
        if(isLengthValid(txref.length()) && (txref.at(0) == 'r' || txref.at(0) == 'y')) {
            return std::string(txref::BECH32_HRP_MAIN) + bech32::separator + txref;
        }
        if(isLengthValid(txref.length()) && (txref.at(0) == 'x' || txref.at(0) == '8')) {
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
        std::string output = prettyPrint(result, hrp.length());

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
        checkExtendedMagicCode(magicCode);

        // ranges have been checked. make unsigned copies of params
        auto bh = static_cast<uint32_t>(blockHeight);
        auto tp = static_cast<uint32_t>(transactionPosition);
        auto ti = static_cast<uint32_t>(txoIndex);

        std::vector<unsigned char> dp(DATA_EXTENDED_SIZE);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);  // sets 1-3 bits in the 1st 5 bits

        // set version bit to 0
        dp[1] &= ~(1u << 0u);                     // sets 1 bit in 2nd 5 bits

        // set block height
        dp[1] |= (bh & 0xFu) << 1u;               // sets 4 bits in 3rd 5 bits
        dp[2] |= (bh & 0x1F0u) >> 4u;             // sets 5 bits in 4th 5 bits
        dp[3] |= (bh & 0x3E00u) >> 9u;            // sets 5 bits in 5th 5 bits
        dp[4] |= (bh & 0x7C000u) >> 14u;          // sets 5 bits in 6th 5 bits
        dp[5] |= (bh & 0xF80000u) >> 19u;         // sets 5 bits in 7th 5 bits (24 bits total for blockHeight)

        // set transaction position
        dp[6] |= (tp & 0x1Fu);                    // sets 5 bits in 8th 5 bits
        dp[7] |= (tp & 0x3E0u) >> 5u;             // sets 5 bits in 9th 5 bits
        dp[8] |= (tp & 0x7C00u) >> 10u;           // sets 5 bits in 10th 5 bits (15 bits total for transactionPosition)

        // set txo index
        dp[9] |= ti & 0x1Fu;                      // sets 5 bits in 11th 5 bits
        dp[10] |= (ti & 0x3E0u) >> 5u;            // sets 5 bits in 12th 5 bits
        dp[11] |= (ti & 0x7C00u) >> 10u;          // sets 5 bits in 13th 5 bits (15 bits total for txoIndex)

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = prettyPrint(result, hrp.length());

        return output;
    }

}

namespace txref {

    std::string encode(
            int blockHeight,
            int transactionPosition,
            int txoIndex,
            bool forceExtended,
            const std::string & hrp) {

        if(txoIndex == 0 && !forceExtended)
            return txrefEncode(hrp, MAGIC_BTC_MAIN, blockHeight, transactionPosition);

        return txrefExtEncode(hrp, MAGIC_BTC_MAIN_EXTENDED, blockHeight, transactionPosition, txoIndex);

    }

    std::string encodeTestnet(
            int blockHeight,
            int transactionPosition,
            int txoIndex,
            bool forceExtended,
            const std::string & hrp) {

        if(txoIndex == 0 && !forceExtended)
            return txrefEncode(hrp, MAGIC_BTC_TEST, blockHeight, transactionPosition);

        return txrefExtEncode(hrp, MAGIC_BTC_TEST_EXTENDED, blockHeight, transactionPosition, txoIndex);

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
        data.txref = prettyPrint(txrefClean, bs.hrp.length());
        data.hrp = bs.hrp;
        data.magicCode = magicCode;
        extractBlockHeight(data.blockHeight, bs);
        extractTransactionPosition(data.transactionPosition, bs);
        extractTxoIndex(data.txoIndex, bs);

        return data;
    }

}

// C bindings - functions

const char *txref_errordesc[] = {
        "Success",
        "Unknown error",
        "Function argument was null",
        "Function argument length was too short",
        "Max error"
};

/**
 * Returns error message string corresponding to the error code
 *
 * @param error_code the error code to convert
 *
 * @return error message string corresponding to the error code
 */
extern "C"
const char * txref_strerror(txref_error error_code) {
    const char * result = "";
    if(error_code >= E_TXREF_SUCCESS && error_code < E_TXREF_MAX_ERROR) {
        result = txref_errordesc[error_code];
    }
    else
        result = txref_errordesc[E_TXREF_UNKNOWN_ERROR];
    return result;
}

/**
 * Get MAX length any txref can be: main/testnet, regular or extended
 *
 * The length includes the final '\0' character
 *
 * @return MAX length for any txref
 */
extern "C"
size_t max_Txref_length() {
    return static_cast<size_t>(txref::limits::TXREF_MAX_LENGTH + 1);
}


/**
 * Allocates memory for a txref and returns a pointer.
 *
 * This memory will be able to handle any size txref.
 *
 * This memory must be freed using the free_Txref_storage function.
 *
 * @return a pointer to a new txref, or NULL if error
 */
extern "C"
char * create_Txref_storage() {
    // allocate enough memory for the largest possible txref string, plus 1 for '\0'
    return static_cast<char *> (calloc(max_Txref_length(), 1));
}

/**
 * Frees memory for a txref
 */
 extern "C"
void free_Txref_storage(char *txref) {
     free(txref);
 }

/**
 * Allocates memory for a txref_LocationData struct and returns a pointer.
 *
 * This struct will be able to handle any size txref.
 *
 * This memory must be freed using the free_LocationData_storage function.
 *
 * @return a pointer to a new txref_LocationData struct, or NULL if error
 */
 extern "C"
txref_LocationData * create_LocationData_storage() {
    auto locationData =
            static_cast<txref_LocationData *> (calloc(1, sizeof(txref_LocationData)));
    if(locationData == nullptr)
        return nullptr;
    locationData->txreflen = size_t(txref::limits::TXREF_MAX_LENGTH) + 1;
    locationData->txref = create_Txref_storage();
    if(locationData->txref == nullptr) {
        free(locationData);
        return nullptr;
    }
    locationData->hrplen = (txref::limits::TXREF_EXT_STRING_MIN_LENGTH_TESTNET - txref::limits::TXREF_EXT_STRING_NO_HRP_MIN_LENGTH) + 1;
    locationData->hrp = static_cast<char *> (calloc(locationData->hrplen, 1));
    if(locationData->hrp == nullptr) {
        free_Txref_storage(locationData->txref);
        free(locationData);
        return nullptr;
    }
    return locationData;
}

/**
 * Frees memory for a txref_LocationData struct.
 */
void free_LocationData_storage(txref_LocationData *locationData) {
    free(locationData->hrp);
    free_Txref_storage(locationData->txref);
    free(locationData);
}

/**
 * encodes the position of a confirmed bitcoin transaction on the
 * mainnet network and returns a bech32 encoded "transaction position
 * reference" (txref). If txoIndex is greater than 0, then an extended
 * reference is returned (txref-ext). If txoIndex is zero, but
 * forceExtended=true, then an extended reference is returned (txref-ext).
 *
 * @param txref pointer to memory to copy the output encoded txref
 * @param txreflen number of bytes allocated at txref
 * @param blockHeight the block height of block containing the transaction to encode
 * @param transactionPosition the transaction position within the block of the transaction to encode
 * @param txoIndex the txo index within the transaction of the transaction to encode
 * @param forceExtended if true, will encode an extended txref, even if txoIndex is 0
 * @param hrp the "human-readable part" for the bech32 encoding (normally "txtest")
 * @param hrplen the length of the "human-readable part" string
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern "C"
txref_error txref_encode(
        char * txref,
        size_t txreflen,
        int blockHeight,
        int transactionPosition,
        int txoIndex,
        bool forceExtended,
        const char * hrp,
        size_t hrplen) {

    if(txref == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(hrp == nullptr)
        return E_TXREF_NULL_ARGUMENT;

    std::string inputHrp(hrp);
    if(inputHrp.size() > hrplen-1)
        return E_TXREF_LENGTH_TOO_SHORT;

    std::string outputTxref;
    try {
        outputTxref = txref::encode(blockHeight, transactionPosition, txoIndex, forceExtended, inputHrp);
    } catch (std::exception &) {
        // todo: convert exception message
        return E_TXREF_UNKNOWN_ERROR;
    }

    if(outputTxref.size() > txreflen-1)
        return E_TXREF_LENGTH_TOO_SHORT;

    std::copy_n(outputTxref.begin(), outputTxref.size(), txref);
    txref[outputTxref.size()] = '\0';

    return E_TXREF_SUCCESS;
}

/**
 * encodes the position of a confirmed bitcoin transaction on the
 * mainnet network and returns a bech32 encoded "transaction position
 * reference" (txref). If txoIndex is greater than 0, then an extended
 * reference is returned (txref-ext). If txoIndex is zero, but
 * forceExtended=true, then an extended reference is returned (txref-ext).
 *
 * @param txref pointer to memory to copy the output encoded txref
 * @param txreflen number of bytes allocated at txref
 * @param blockHeight the block height of block containing the transaction to encode
 * @param transactionPosition the transaction position within the block of the transaction to encode
 * @param txoIndex the txo index within the transaction of the transaction to encode
 * @param forceExtended if true, will encode an extended txref, even if txoIndex is 0
 * @param hrp the "human-readable part" for the bech32 encoding (normally "txtest")
 * @param hrplen the length of the "human-readable part" string
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern "C"
txref_error txref_encodeTestnet(
        char * txref,
        size_t txreflen,
        int blockHeight,
        int transactionPosition,
        int txoIndex,
        bool forceExtended,
        const char * hrp,
        size_t hrplen) {

    if(txref == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(hrp == nullptr)
        return E_TXREF_NULL_ARGUMENT;

    std::string inputHrp(hrp);
    if(inputHrp.size() > hrplen-1)
        return E_TXREF_LENGTH_TOO_SHORT;

    std::string outputTxref;
    try {
        outputTxref = txref::encodeTestnet(blockHeight, transactionPosition, txoIndex, forceExtended, inputHrp);
    } catch (std::exception &) {
        // todo: convert exception message
        return E_TXREF_UNKNOWN_ERROR;
    }

    if(outputTxref.size() > txreflen-1)
        return E_TXREF_LENGTH_TOO_SHORT;

    std::copy_n(outputTxref.begin(), outputTxref.size(), txref);
    txref[outputTxref.size()] = '\0';

    return E_TXREF_SUCCESS;
}

/**
 * decodes a bech32 encoded "transaction position reference" (txref) and
 * returns identifying data
 *
 * @param locationData pointer to struct to copy the decoded transaction data
 * @param txref the txref string to decode
 * @param txreflen the length of the txref string
 *
 * @return E_TXREF_SUCCESS on success, others on error
 */
extern "C"
txref_error txref_decode(
        txref_LocationData *locationData,
        const char * txref,
        size_t txreflen) {

    if(locationData == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(locationData->txref == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(locationData->hrp == nullptr)
        return E_TXREF_NULL_ARGUMENT;
    if(txref == nullptr)
        return E_TXREF_NULL_ARGUMENT;

    std::string inputTxref(txref);
    if(inputTxref.size() > txreflen-1)
        return E_TXREF_LENGTH_TOO_SHORT;

    txref::LocationData ld;
    try {
        ld = txref::decode(inputTxref);
    } catch (std::exception &) {
        // todo: convert exception message
        return E_TXREF_UNKNOWN_ERROR;
    }

    locationData->magicCode = ld.magicCode;
    locationData->blockHeight = ld.blockHeight;
    locationData->transactionPosition = ld.transactionPosition;
    locationData->txoIndex = ld.txoIndex;

    std::copy_n(ld.hrp.begin(), ld.hrp.size(), locationData->hrp);
    locationData->hrp[ld.hrp.size()] = '\0';

    std::copy_n(ld.txref.begin(), ld.txref.size(), locationData->txref);
    locationData->txref[ld.txref.size()] = '\0';

    return E_TXREF_SUCCESS;
}
