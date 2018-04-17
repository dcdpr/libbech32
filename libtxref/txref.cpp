
#include "txref.h"
#include "../libbech32/bech32.h"
#include <vector>
#include <stdexcept>

namespace {

    const int MAX_BLOCK_HEIGHT         = 0x1FFFFF;
    const int MAX_BLOCK_HEIGHT_TESTNET = 0x3FFFFFF;

    const int MAX_TRANSACTION_POSITION         = 0x1FFF;
    const int MAX_TRANSACTION_POSITION_TESTNET = 0x3FFFF;

    const int MAX_UTXO_INDEX           = 0x1FFF;

    const int MAX_MAGIC_CODE = 0x1F;

    const int DATA_SIZE = 8;
    const int DATA_SIZE_TESTNET = 10;

    const int DATA_EXTENDED_SIZE = 11;
    const int DATA_EXTENDED_SIZE_TESTNET = 13;


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

    // a UXTO's index can only be in a certain range
    void checkUtxoIndexRange(int utxoIndex) {
        if(utxoIndex < 0 || utxoIndex > MAX_UTXO_INDEX)
            throw std::runtime_error("utxo index is too large");
    }

    // a transaction's position can only be in a certain range for testnet
    void checkTransactionPositionRangeTestnet(int transactionPosition) {
        if(transactionPosition < 0 || transactionPosition > MAX_TRANSACTION_POSITION_TESTNET)
            throw std::runtime_error("transaction position is too large");
    }

    // the magic code can only be in a certain range
    void checkMagicCodeRange(char magicCode) {
        if(magicCode < 0 || magicCode > MAX_MAGIC_CODE)
            throw std::runtime_error("magic code is too large");
    }

    // add dashes to the txref string to make it look nicer
    std::string addDashes(const std::string & raw, std::string::size_type hrplen) {
        std::string output(raw);
        output.insert(output.begin()+hrplen+1, '-');
        output.insert(output.begin()+hrplen+6, '-');
        output.insert(output.begin()+hrplen+11, '-');
        output.insert(output.begin()+hrplen+16, '-');
        return output;
    }

    // extract the block height from the decoded data part
    void extractBlockHeight(int & blockHeight, const bech32::HrpAndDp &hd) {
        blockHeight = (hd.dp[1] >> 1);
        blockHeight |= (hd.dp[2] << 4);
        blockHeight |= (hd.dp[3] << 9);
        blockHeight |= (hd.dp[4] << 14);
        auto dataSize = hd.dp.size();
        if (dataSize == 8) {
            blockHeight |= ((hd.dp[5] & 0x03) << 19);
        }
        else {
            blockHeight |= (hd.dp[5] << 19);
            blockHeight |= ((hd.dp[6] & 0x03) << 24);
        }
    }

    // extract the transaction position from the decoded data part
    void extractTransactionPosition(int & transactionPosition, const bech32::HrpAndDp &hd) {
        auto dataSize = hd.dp.size();
        if (dataSize == 8 || dataSize == 11) {
            transactionPosition = ((hd.dp[5] & 0x1C) >> 2);
            transactionPosition |= (hd.dp[6] << 3);
            transactionPosition |= (hd.dp[7] << 8);
        }
        else if (dataSize == 10 || dataSize == 13) {
            transactionPosition = ((hd.dp[6] & 0x1C) >> 2);
            transactionPosition |= (hd.dp[7] << 3);
            transactionPosition |= (hd.dp[8] << 8);
            transactionPosition |= (hd.dp[9] << 13);
        }
    }

    // extract the UXTO index from the decoded data part
    void extractUtxoIndex(int & uxtoIndex, const bech32::HrpAndDp &hd) {
        auto dataSize = hd.dp.size();
        if (dataSize == 11) {
            uxtoIndex = hd.dp[8];
            uxtoIndex |= (hd.dp[9] << 5);
            uxtoIndex |= (hd.dp[10] << 10);
        }
        else if (dataSize == 13) {
            uxtoIndex = hd.dp[10];
            uxtoIndex |= (hd.dp[11] << 5);
            uxtoIndex |= (hd.dp[12] << 10);
        }
    }

    // some txref strings may have had the HRP stripped off. Attempt to prepend one if needed.
    std::string addHrpIfNeeded(const std::string & txref) {
        if(txref.length() == 14 && txref.at(0) == 'r') {
            return std::string(txref::BECH32_HRP_MAIN) + bech32::separator + txref;
        }
        if(txref.length() == 16 && txref.at(0) == 'x') {
            return std::string(txref::BECH32_HRP_TEST) + bech32::separator + txref;
        }
        return txref;
    }

}

namespace txref {

    std::string bitcoinTxrefEncode(
            const std::string & hrp,
            char magicCode,
            int blockHeight,
            int transactionPosition) {

        checkBlockHeightRange(blockHeight);
        checkTransactionPositionRange(transactionPosition);
        checkMagicCodeRange(magicCode);

        std::vector<unsigned char> dp(DATA_SIZE);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);

        // set version bit to 0
        dp[1] &= ~(1 << 0);

        // set block height and transaction position
        dp[1] |= ((blockHeight & 0xF) << 1);
        dp[2] |= ((blockHeight & 0x1F0) >> 4);
        dp[3] |= ((blockHeight & 0x3E00) >> 9);
        dp[4] |= ((blockHeight & 0x7C000) >> 14);

        dp[5] |= ((blockHeight & 0x180000) >> 19);
        dp[5] |= ((transactionPosition & 0x7) << 2);
        dp[6] |= ((transactionPosition & 0xF8) >> 3);
        dp[7] |= ((transactionPosition & 0x1F00) >> 8);


        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = addDashes(result, hrp.length());

        return output;
    }

    std::string bitcoinTxrefExtEncode(
            const std::string & hrp,
            char magicCode,
            int blockHeight,
            int transactionPosition,
            int utxoIndex) {

        checkBlockHeightRange(blockHeight);
        checkTransactionPositionRange(transactionPosition);
        checkUtxoIndexRange(utxoIndex);
        checkMagicCodeRange(magicCode);

        std::vector<unsigned char> dp(DATA_EXTENDED_SIZE);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);              // sets 1-3 bits in the 1st 5 bits

        // set version bit to 0 (clear bit)
        dp[1] &= ~(1 << 0);                                  // sets 1 bit in 2nd 5 bits

        // set block height and transaction position
        dp[1] |= ((blockHeight & 0xF) << 1);                 // sets 4 bits in 2nd 5 bits
        dp[2] |= ((blockHeight & 0x1F0) >> 4);               // sets 5 bits in 3rd 5 bits
        dp[3] |= ((blockHeight & 0x3E00) >> 9);              // sets 5 bits in 4th 5 bits
        dp[4] |= ((blockHeight & 0x7C000) >> 14);            // sets 5 bits in 5th 5 bits

        dp[5] |= ((blockHeight & 0x180000) >> 19);           // sets 2 bits in 6th 5 bits (21 bits total for blockHeight)
        dp[5] |= ((transactionPosition & 0x7) << 2);         // sets 3 bits in 6th 5 bits
        dp[6] |= ((transactionPosition & 0xF8) >> 3);        // sets 5 bits in 7th 5 bits
        dp[7] |= ((transactionPosition & 0x1F00) >> 8);      // sets 5 bits in 8th 5 bits (13 bits total for transactionPosition)

        dp[8] |= ((utxoIndex & 0x1F));                       // sets 5 bits in 9th 5 bits
        dp[9] |= ((utxoIndex & 0x3E0) >> 5);                 // sets 5 bits in 10th 5 bits
        dp[10] |= ((utxoIndex & 0x1C00) >> 10);              // sets 3 bits in 11th 5 bits

        // clear last two bits for now
        dp[10] &= ~(1 << 4);
        dp[10] &= ~(1 << 5);

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = addDashes(result, hrp.length());

        return output;
    }

    std::string bitcoinTxrefEncodeTestnet(
            const std::string & hrp,
            char magicCode,
            int blockHeight,
            int transactionPosition) {

        checkBlockHeightRangeTestnet(blockHeight);
        checkTransactionPositionRangeTestnet(transactionPosition);
        checkMagicCodeRange(magicCode);

        std::vector<unsigned char> dp(DATA_SIZE_TESTNET);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);

        // set version bit to 0
        dp[1] &= ~(1 << 0);

        // set block height and transaction position
        dp[1] |= ((blockHeight & 0xF) << 1);
        dp[2] |= ((blockHeight & 0x1F0) >> 4);
        dp[3] |= ((blockHeight & 0x3E00) >> 9);
        dp[4] |= ((blockHeight & 0x7C000) >> 14);


        dp[5] |= ((blockHeight & 0xF80000) >> 19);
        dp[6] |= ((blockHeight & 0x3000000) >> 24);
        dp[6] |= ((transactionPosition & 0x7) << 2);
        dp[7] |= ((transactionPosition & 0xF8) >> 3);
        dp[8] |= ((transactionPosition & 0x1F00) >> 8);
        dp[9] |= ((transactionPosition & 0x3E000) >> 13);


        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = addDashes(result, hrp.length());

        return output;
    }

    std::string bitcoinTxrefExtEncodeTestnet(
            const std::string & hrp,
            char magicCode,
            int blockHeight,
            int transactionPosition,
            int utxoIndex) {

        checkBlockHeightRangeTestnet(blockHeight);
        checkTransactionPositionRangeTestnet(transactionPosition);
        checkUtxoIndexRange(utxoIndex);
        checkMagicCodeRange(magicCode);

        std::vector<unsigned char> dp(DATA_EXTENDED_SIZE_TESTNET);

        // set the magic code
        dp[0] = static_cast<uint8_t>(magicCode);

        // set version bit to 0
        dp[1] &= ~(1 << 0);

        // set block height and transaction position
        dp[1] |= ((blockHeight & 0xF) << 1);
        dp[2] |= ((blockHeight & 0x1F0) >> 4);
        dp[3] |= ((blockHeight & 0x3E00) >> 9);
        dp[4] |= ((blockHeight & 0x7C000) >> 14);


        dp[5] |= ((blockHeight & 0xF80000) >> 19);
        dp[6] |= ((blockHeight & 0x3000000) >> 24);
        dp[6] |= ((transactionPosition & 0x7) << 2);
        dp[7] |= ((transactionPosition & 0xF8) >> 3);
        dp[8] |= ((transactionPosition & 0x1F00) >> 8);
        dp[9] |= ((transactionPosition & 0x3E000) >> 13);

        dp[10] |= ((utxoIndex & 0x1F));                       // sets 5 bits in 9th 5 bits
        dp[11] |= ((utxoIndex & 0x3E0) >> 5);                 // sets 5 bits in 10th 5 bits
        dp[12] |= ((utxoIndex & 0x1C00) >> 10);              // sets 3 bits in 11th 5 bits

        // clear last two bits for now
        dp[12] &= ~(1 << 4);
        dp[12] &= ~(1 << 5);

        // Bech32 encode
        std::string result = bech32::encode(hrp, dp);

        // add the dashes
        std::string output = addDashes(result, hrp.length());

        return output;
    }

    LocationData bitcoinTxrefDecode(const std::string & txref) {

        std::string txrefClean = bech32::stripUnknownChars(txref);
        txrefClean = addHrpIfNeeded(txrefClean);
        bech32::HrpAndDp bs = bech32::decode(txrefClean);

        auto dataSize = bs.dp.size();
        if(dataSize != 8 && dataSize != 10 && dataSize != 11 && dataSize != 13) {
            throw std::runtime_error("decoded dp size is incorrect");
        }

        LocationData data;
        data.txref = addDashes(txrefClean, bs.hrp.length());
        data.hrp = bs.hrp;
        data.magicCode = bs.dp[0];
        extractBlockHeight(data.blockHeight, bs);
        extractTransactionPosition(data.transactionPosition, bs);
        extractUtxoIndex(data.uxtoIndex, bs);

        return data;
    }

}
