
#include "txref.h"
#include "../libbech32/bech32.h"
#include <vector>
#include <stdexcept>

namespace {

    const int MAX_BLOCK_HEIGHT         = 0x1FFFFF;
    const int MAX_BLOCK_HEIGHT_TESTNET = 0x3FFFFFF;

    const int MAX_TRANSACTION_POSITION         = 0x1FFF;
    const int MAX_TRANSACTION_POSITION_TESTNET = 0x3FFFF;

    const int MAX_MAGIC_CODE = 0x1F;

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
        if (dataSize == 8) {
            transactionPosition = ((hd.dp[5] & 0x1C) >> 2);
            transactionPosition |= (hd.dp[6] << 3);
            transactionPosition |= (hd.dp[7] << 8);
        }
        else {
            transactionPosition = ((hd.dp[6] & 0x1C) >> 2);
            transactionPosition |= (hd.dp[7] << 3);
            transactionPosition |= (hd.dp[8] << 8);
            transactionPosition |= (hd.dp[9] << 13);
        }
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

        std::vector<unsigned char> dp(8);

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

    std::string bitcoinTxrefEncodeTestnet(
            const std::string & hrp,
            char magicCode,
            int blockHeight,
            int transactionPosition) {

        checkBlockHeightRangeTestnet(blockHeight);
        checkTransactionPositionRangeTestnet(transactionPosition);
        checkMagicCodeRange(magicCode);

        std::vector<unsigned char> dp(10);

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

    LocationData bitcoinTxrefDecode(const std::string & txref) {

        std::string txrefClean = bech32::stripUnknownChars(txref);
        bech32::HrpAndDp bs = bech32::decode(txrefClean);

        auto dataSize = bs.dp.size();
        if(dataSize != 8 && dataSize != 10) {
            throw std::runtime_error("decoded dp size is incorrect");
        }

        LocationData data;
        data.hrp = bs.hrp;
        data.magicCode = bs.dp[0];
        extractBlockHeight(data.blockHeight, bs);
        extractTransactionPosition(data.transactionPosition, bs);

        return data;
    }

}
