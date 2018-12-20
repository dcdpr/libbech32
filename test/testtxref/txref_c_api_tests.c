// test program calling txref library from C

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "libtxref.h"

// make sure we can run these tests even when building a release version
#undef NDEBUG
#include <assert.h>

void strerror_withValidErrorCode_returnsErrorMessage() {
    const char *message = txref_strerror(E_TXREF_SUCCESS);
    assert(message != NULL);
    assert(strcmp(message, "Success") == 0);
}

void strerror_withInvalidErrorCode_returnsUnknownErrorMessage() {
    const char *message = txref_strerror(1234);
    assert(message != NULL);
    assert(strcmp(message, "Unknown error") == 0);
}

void encode_withBadArgs_isUnsuccessful() {
    { // input is null
        char foo[] = "foo";
        assert(txref_encode(foo, sizeof(foo), 10, 10, 10, false, NULL, 0) == E_TXREF_NULL_ARGUMENT);
    }

    { // output is null
        char foo[] = "foo";
        assert(txref_encode(NULL, 0, 10, 10, 10, false, foo, sizeof(foo)) == E_TXREF_NULL_ARGUMENT);
    }

    { // allocated output is too short
        char hrp[] = "tx";
        const int txreflen = 22; // for mainnet, this should be 23 = 15 data chars + 1 ':' + 3 '-' + 'tx1' + '\0';
        char txref[txreflen];
        assert(txref_encode(txref, txreflen, 0, 0, 0, false, hrp, sizeof(hrp)) == E_TXREF_LENGTH_TOO_SHORT);
    }
}

void encode_whenMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to encode an HRP string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char hrp[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    const int txreflen = 23;
    char txref[txreflen];
    assert(txref_encode(txref, txreflen, 0, 0, 0, false, hrp, sizeof(hrp)) == E_TXREF_UNKNOWN_ERROR);
}

void encode_mainnetExamples_areSuccessful() {
    char hrp[] = "tx";

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    struct examples mainnet_examples[] = {
            { 0, 0, 0, "tx1:rqqq-qqqq-qmhu-qhp" },
            { 0, 0x7FFF, 0, "tx1:rqqq-qqll-l8xh-jkg" },
            { 0xFFFFFF, 0x7FFF, 0, "tx1:r7ll-llll-l5xt-jzw" },
            { 466793, 2205, 0, "tx1:rjk0-uqay-zsrw-hqe" },
    };
    int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = mainnet_examples[example_index].txref;
        const size_t expectedTxrefSize = strlen(expectedTxref)+1; // 23 = 15 data chars + 1 ':' + 3 '-' + 'tx1' + '\0';
        char *txref = (char *) calloc(expectedTxrefSize, 1);

        assert(txref_encode(
                txref, expectedTxrefSize,
                mainnet_examples[example_index].blockHeight,
                mainnet_examples[example_index].transactionPosition,
                mainnet_examples[example_index].txoIndex, false, hrp, sizeof(hrp)) == E_TXREF_SUCCESS);
        assert(strcmp(txref, expectedTxref) == 0);

        free(txref);
    }
}

void encode_mainnetExtendedExamples_areSuccessful() {
    char hrp[] = "tx";

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    struct examples mainnet_examples[] = {
            { 0, 0, 100, "tx1:yqqq-qqqq-qyrq-0ks7-gt" },
            { 0, 0, 0x1FFF, "tx1:yqqq-qqqq-qll8-p5t3-qa" },
            { 0, 0x1FFF, 100, "tx1:yqqq-qqll-8yrq-6d7j-km" },
            { 0x1FFFFF, 0, 200, "tx1:y7ll-lrqq-qgxq-2dcs-u5" },
            { 0x1FFFFF, 0x1FFF, 0x1FFF, "tx1:y7ll-lrll-8ll8-n7qg-vg" },
            { 466793, 2205, 10, "tx1:yjk0-uqay-z2qq-km8k-yl" },
            { 466793, 2205, 0x1FFF, "tx1:yjk0-uqay-zll8-4tgt-z7" }
    };
    int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = mainnet_examples[example_index].txref;
        const size_t expectedTxrefSize = strlen(expectedTxref)+1; // 27 = 18 data chars + 1 ':' + 4 '-' + 'tx1' + '\0';
        char *txref = (char *) calloc(expectedTxrefSize, 1);

        assert(txref_encode(
                txref, expectedTxrefSize,
                mainnet_examples[example_index].blockHeight,
                mainnet_examples[example_index].transactionPosition,
                mainnet_examples[example_index].txoIndex, false, hrp, sizeof(hrp)) == E_TXREF_SUCCESS);
        assert(strcmp(txref, expectedTxref) == 0);

        free(txref);
    }
}

void encode_testnetExamples_areSuccessful() {
    char hrp[] = "txtest";

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    struct examples testnet_examples[] = {
            { 0, 0, 0, "txtest1:xqqq-qqqq-qkla-64l"},
            { 0xFFFFFF, 0x7FFF, 0, "txtest1:x7ll-llll-lew2-gqs"},
            { 467883, 2355, 0, "txtest1:xk63-uqnf-zasf-wgq"},
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = testnet_examples[example_index].txref;
        const size_t expectedTxrefSize = strlen(expectedTxref)+1; // 27 = 15 data chars + 1 ':' + 3 '-' + 'txtest1' + '\0';
        char *txref = (char *) calloc(expectedTxrefSize, 1);

        assert(txref_encodeTestnet(
                txref, expectedTxrefSize,
                testnet_examples[example_index].blockHeight,
                testnet_examples[example_index].transactionPosition,
                testnet_examples[example_index].txoIndex, false, hrp, sizeof(hrp)) == E_TXREF_SUCCESS);
        assert(strcmp(txref, expectedTxref) == 0);

        free(txref);
    }
}

void encode_testnetExtendedExamples_areSuccessful() {
    char hrp[] = "txtest";

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    struct examples testnet_examples[] = {
            { 0, 0, 0x1FFF, "txtest1:8qqq-qqqq-qll8-0vy6-r6" },
            { 0, 0x7FFF, 100, "txtest1:8qqq-qqll-lyrq-m3yg-tq" },
            { 0xFFFFFF, 0, 200, "txtest1:87ll-llqq-qgxq-gqua-v5" },
            { 0xFFFFFF, 0x7FFF, 0x1FFF, "txtest1:87ll-llll-lll8-7h35-z5" },
            { 466793, 2205, 10, "txtest1:8jk0-uqay-z2qq-crga-8c" },
            { 466793, 2205, 0x1FFF, "txtest1:8jk0-uqay-zll8-mn8q-pe" }
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char *expectedTxref = testnet_examples[example_index].txref;
        const size_t expectedTxrefSize = strlen(expectedTxref)+1; // 31 = 18 data chars + 1 ':' + 4 '-' + 'txtest1' + '\0';
        char *txref = (char *) calloc(expectedTxrefSize, 1);

        assert(txref_encodeTestnet(
                txref, expectedTxrefSize,
                testnet_examples[example_index].blockHeight,
                testnet_examples[example_index].transactionPosition,
                testnet_examples[example_index].txoIndex, false, hrp, sizeof(hrp)) == E_TXREF_SUCCESS);
        assert(strcmp(txref, expectedTxref) == 0);

        free(txref);
    }
}

void encode_AnyExamples_areSuccessful() {
    // this test function repeats some encoding tests above, but here we are testing that
    // the storage allocation functions are able to allocate enough memory to safely handle
    // any kind of txref (regular or extended)

    struct examples {
        int blockHeight, transactionPosition, txoIndex;
        const char *txref;
    };

    { // mainnet
        struct examples mainnet_examples[] = {
                {466793, 2205, 0,  "tx1:rjk0-uqay-zsrw-hqe"},
                {466793, 2205, 10, "tx1:yjk0-uqay-z2qq-km8k-yl"}
        };
        int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

        char hrp[] = "tx";
        size_t hrplen = sizeof(hrp);

        for (int example_index = 0; example_index < num_examples; example_index++) {
            const char *expectedTxref = mainnet_examples[example_index].txref;

            const size_t expectedTxrefSize = max_Txref_length();
            char *txref = create_Txref_storage();

            assert(txref_encode(
                    txref, expectedTxrefSize,
                    mainnet_examples[example_index].blockHeight,
                    mainnet_examples[example_index].transactionPosition,
                    mainnet_examples[example_index].txoIndex, false, hrp, hrplen) == E_TXREF_SUCCESS);
            assert(strcmp(txref, expectedTxref) == 0);

            free_Txref_storage(txref);
        }
    }

    { // testnet
        struct examples testnet_examples[] = {
                {467883, 2355, 0,  "txtest1:xk63-uqnf-zasf-wgq"},
                {466793, 2205, 10, "txtest1:8jk0-uqay-z2qq-crga-8c"}
        };
        int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

        char hrp[] = "txtest";
        size_t hrplen = sizeof(hrp);

        for (int example_index = 0; example_index < num_examples; example_index++) {
            const char *expectedTxref = testnet_examples[example_index].txref;

            const size_t expectedTxrefSize = max_Txref_length();
            char *txref = create_Txref_storage();

            assert(txref_encodeTestnet(
                    txref, expectedTxrefSize,
                    testnet_examples[example_index].blockHeight,
                    testnet_examples[example_index].transactionPosition,
                    testnet_examples[example_index].txoIndex, false, hrp, hrplen) == E_TXREF_SUCCESS);
            assert(strcmp(txref, expectedTxref) == 0);

            free_Txref_storage(txref);
        }
    }
}

void decode_withBadArgs_isUnsuccessful() {

    { // output is null
        char txref[] = "foo";
        assert(txref_decode(NULL, txref, sizeof(txref)) == E_TXREF_NULL_ARGUMENT);
    }

    { // input is null
        txref_LocationData *locationData = create_LocationData_storage();
        assert(txref_decode(locationData, NULL, 0) == E_TXREF_NULL_ARGUMENT);
        free_LocationData_storage(locationData);
    }

    { // locationData->txref is null
        char txref[] = "foo";
        txref_LocationData *locationData = create_LocationData_storage();
        free_Txref_storage(locationData->txref);
        locationData->txref = NULL;
        assert(txref_decode(locationData, txref, sizeof(txref)) == E_TXREF_NULL_ARGUMENT);
        free_LocationData_storage(locationData);
    }

    { // locationData->hrp is null
        char txref[] = "foo";
        txref_LocationData *locationData = create_LocationData_storage();
        free(locationData->hrp);
        locationData->hrp = NULL;
        assert(txref_decode(locationData, txref, sizeof(txref)) == E_TXREF_NULL_ARGUMENT);
        free_LocationData_storage(locationData);
    }
}

void decode_whenMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to decode a string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    char txref[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    txref_LocationData *locationData = create_LocationData_storage();
    assert(txref_decode(locationData, txref, sizeof(txref)) == E_TXREF_UNKNOWN_ERROR);
    free_LocationData_storage(locationData);
}

void decode_mainnetExamples_areSuccessful() {
    char hrp[] = "tx";

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    struct examples mainnet_examples[] = {
            { "tx1:rqqq-qqqq-qmhu-qhp", 0, 0, 0},
            { "tx1:rqqq-qqll-l8xh-jkg", 0, 0x7FFF, 0},
            { "tx1:r7ll-llll-l5xt-jzw", 0xFFFFFF, 0x7FFF, 0},
            { "tx1:rjk0-uqay-zsrw-hqe", 466793, 2205, 0},
    };
    int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = mainnet_examples[example_index].txref;
        txref_LocationData *locationData =
                (txref_LocationData *) calloc(1, sizeof(txref_LocationData));
        locationData->txreflen = strlen(txref)+1;
        locationData->txref = (char *) calloc(locationData->txreflen, 1);
        locationData->hrplen = sizeof(hrp);
        locationData->hrp = (char *) calloc(locationData->hrplen, 1);

        assert(txref_decode(locationData, txref, strlen(txref)+1) == E_TXREF_SUCCESS);
        assert(locationData->blockHeight == mainnet_examples[example_index].blockHeight);
        assert(locationData->transactionPosition == mainnet_examples[example_index].transactionPosition);
        assert(locationData->txoIndex == mainnet_examples[example_index].txoIndex);
        assert(strcmp(locationData->txref, txref) == 0);
        assert(strcmp(locationData->hrp, hrp) == 0);

        free(locationData->hrp);
        free(locationData->txref);
        free(locationData);
    }
}

void decode_mainnetExtendedExamples_areSuccessful() {
    char hrp[] = "tx";

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    struct examples mainnet_examples[] = {
            { "tx1:yqqq-qqqq-qyrq-0ks7-gt", 0, 0, 100 },
            { "tx1:yqqq-qqqq-qll8-p5t3-qa", 0, 0, 0x1FFF },
            { "tx1:yqqq-qqll-8yrq-6d7j-km", 0, 0x1FFF, 100 },
            { "tx1:y7ll-lrqq-qgxq-2dcs-u5", 0x1FFFFF, 0, 200 },
            { "tx1:y7ll-lrll-8ll8-n7qg-vg", 0x1FFFFF, 0x1FFF, 0x1FFF },
            { "tx1:yjk0-uqay-z2qq-km8k-yl", 466793, 2205, 10 },
            { "tx1:yjk0-uqay-zll8-4tgt-z7", 466793, 2205, 0x1FFF }
    };
    int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = mainnet_examples[example_index].txref;
        txref_LocationData *locationData =
                (txref_LocationData *) calloc(1, sizeof(txref_LocationData));
        locationData->txreflen = strlen(txref)+1;
        locationData->txref = (char *) calloc(locationData->txreflen, 1);
        locationData->hrplen = sizeof(hrp);
        locationData->hrp = (char *) calloc(locationData->hrplen, 1);

        assert(txref_decode(locationData, txref, strlen(txref)+1) == E_TXREF_SUCCESS);
        assert(locationData->blockHeight == mainnet_examples[example_index].blockHeight);
        assert(locationData->transactionPosition == mainnet_examples[example_index].transactionPosition);
        assert(locationData->txoIndex == mainnet_examples[example_index].txoIndex);
        assert(strcmp(locationData->txref, txref) == 0);
        assert(strcmp(locationData->hrp, hrp) == 0);

        free(locationData->hrp);
        free(locationData->txref);
        free(locationData);
    }
}

void decode_testnetExamples_areSuccessful() {
    char hrp[] = "txtest";

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    struct examples testnet_examples[] = {
            { "txtest1:xqqq-qqqq-qkla-64l", 0, 0, 0},
            { "txtest1:x7ll-llll-lew2-gqs", 0xFFFFFF, 0x7FFF, 0},
            { "txtest1:xk63-uqnf-zasf-wgq", 467883, 2355, 0},
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = testnet_examples[example_index].txref;
        txref_LocationData *locationData =
                (txref_LocationData *) calloc(1, sizeof(txref_LocationData));
        locationData->txreflen = strlen(txref)+1;
        locationData->txref = (char *) calloc(locationData->txreflen, 1);
        locationData->hrplen = sizeof(hrp);
        locationData->hrp = (char *) calloc(locationData->hrplen, 1);

        assert(txref_decode(locationData, txref, strlen(txref)+1) == E_TXREF_SUCCESS);
        assert(locationData->blockHeight == testnet_examples[example_index].blockHeight);
        assert(locationData->transactionPosition == testnet_examples[example_index].transactionPosition);
        assert(locationData->txoIndex == testnet_examples[example_index].txoIndex);
        assert(strcmp(locationData->txref, txref) == 0);
        assert(strcmp(locationData->hrp, hrp) == 0);

        free(locationData->hrp);
        free(locationData->txref);
        free(locationData);
    }
}

void decode_testnetExtendedExamples_areSuccessful() {
    char hrp[] = "txtest";

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    struct examples testnet_examples[] = {
            { "txtest1:8qqq-qqqq-qll8-0vy6-r6", 0, 0, 0x1FFF },
            { "txtest1:8qqq-qqll-lyrq-m3yg-tq", 0, 0x7FFF, 100, },
            { "txtest1:87ll-llqq-qgxq-gqua-v5", 0xFFFFFF, 0, 200 },
            { "txtest1:87ll-llll-lll8-7h35-z5", 0xFFFFFF, 0x7FFF, 0x1FFF },
            { "txtest1:8jk0-uqay-z2qq-crga-8c", 466793, 2205, 10 },
            { "txtest1:8jk0-uqay-zll8-mn8q-pe", 466793, 2205, 0x1FFF }
    };
    int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

    for(int example_index = 0; example_index < num_examples; example_index++) {
        const char * txref = testnet_examples[example_index].txref;
        txref_LocationData *locationData =
                (txref_LocationData *) calloc(1, sizeof(txref_LocationData));
        locationData->txreflen = strlen(txref)+1;
        locationData->txref = (char *) calloc(locationData->txreflen, 1);
        locationData->hrplen = sizeof(hrp);
        locationData->hrp = (char *) calloc(locationData->hrplen, 1);

        assert(txref_decode(locationData, txref, strlen(txref)+1) == E_TXREF_SUCCESS);
        assert(locationData->blockHeight == testnet_examples[example_index].blockHeight);
        assert(locationData->transactionPosition == testnet_examples[example_index].transactionPosition);
        assert(locationData->txoIndex == testnet_examples[example_index].txoIndex);
        assert(strcmp(locationData->txref, txref) == 0);
        assert(strcmp(locationData->hrp, hrp) == 0);

        free(locationData->hrp);
        free(locationData->txref);
        free(locationData);
    }
}

void decode_AnyExtendedExamples_areSuccessful() {
    // this test function repeats some decoding tests above, but here we are testing that
    // the storage allocation functions are able to allocate enough memory to safely handle
    // any kind of txref (regular or extended)

    struct examples {
        const char *txref;
        int blockHeight, transactionPosition, txoIndex;
    };

    { // mainnet
        struct examples mainnet_examples[] = {
                {"tx1:rjk0-uqay-zsrw-hqe",     466793, 2205, 0},
                {"tx1:yjk0-uqay-z2qq-km8k-yl", 466793, 2205, 10}
        };
        int num_examples = sizeof(mainnet_examples) / sizeof(mainnet_examples[0]);

        char hrp[] = "tx";

        for (int example_index = 0; example_index < num_examples; example_index++) {
            const char *txref = mainnet_examples[example_index].txref;
            txref_LocationData *locationData = create_LocationData_storage();

            assert(txref_decode(locationData, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
            assert(locationData->blockHeight == mainnet_examples[example_index].blockHeight);
            assert(locationData->transactionPosition == mainnet_examples[example_index].transactionPosition);
            assert(locationData->txoIndex == mainnet_examples[example_index].txoIndex);
            assert(strcmp(locationData->txref, txref) == 0);
            assert(strcmp(locationData->hrp, hrp) == 0);

            free_LocationData_storage(locationData);
        }
    }

    { // testnet
        struct examples testnet_examples[] = {
                {"txtest1:xk63-uqnf-zasf-wgq",     467883, 2355, 0},
                {"txtest1:8jk0-uqay-z2qq-crga-8c", 466793, 2205, 10}
        };
        int num_examples = sizeof(testnet_examples) / sizeof(testnet_examples[0]);

        char hrp[] = "txtest";

        for (int example_index = 0; example_index < num_examples; example_index++) {
            const char *txref = testnet_examples[example_index].txref;
            txref_LocationData *locationData = create_LocationData_storage();

            assert(txref_decode(locationData, txref, strlen(txref) + 1) == E_TXREF_SUCCESS);
            assert(locationData->blockHeight == testnet_examples[example_index].blockHeight);
            assert(locationData->transactionPosition == testnet_examples[example_index].transactionPosition);
            assert(locationData->txoIndex == testnet_examples[example_index].txoIndex);
            assert(strcmp(locationData->txref, txref) == 0);
            assert(strcmp(locationData->hrp, hrp) == 0);

            free_LocationData_storage(locationData);
        }
    }
}

int main() {

    strerror_withValidErrorCode_returnsErrorMessage();
    strerror_withInvalidErrorCode_returnsUnknownErrorMessage();

    encode_withBadArgs_isUnsuccessful();
    encode_whenMethodThrowsException_isUnsuccessful();
    encode_mainnetExamples_areSuccessful();
    encode_mainnetExtendedExamples_areSuccessful();
    encode_testnetExamples_areSuccessful();
    encode_testnetExtendedExamples_areSuccessful();
    encode_AnyExamples_areSuccessful();

    decode_withBadArgs_isUnsuccessful();
    decode_whenMethodThrowsException_isUnsuccessful();
    decode_mainnetExamples_areSuccessful();
    decode_mainnetExtendedExamples_areSuccessful();
    decode_testnetExamples_areSuccessful();
    decode_testnetExtendedExamples_areSuccessful();
    decode_AnyExtendedExamples_areSuccessful();

    return 0;
}
