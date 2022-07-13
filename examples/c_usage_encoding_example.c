#include "bech32.h"
#include <string.h>
#include <stdio.h>


int main() {
    // simple human readable part with some data
    char hrp[] = "hello";
    unsigned char dp[] = {14, 15, 3, 31, 13};

    // create storage for bech32 string
    bech32_bstring *bstring = bech32_create_bstring(strlen(hrp), sizeof(dp));
    if(!bstring) {
        printf("bech32 string can not be created");
        return E_BECH32_NO_MEMORY;
    }

    // encode
    bech32_error err = bech32_encode(bstring, hrp, dp, sizeof(dp));
    if(err != E_BECH32_SUCCESS) {
        printf("%s\n", bech32_strerror(err));
        bech32_free_bstring(bstring);
        return err;
    }

    printf("bech32 encoding of human-readable part \'hello\' and data part \'[14, 15, 3, 31, 13]\' is:\n");
    printf("%s\n", bstring->string);
    // prints "hello1w0rldjn365x"
    // ... "hello" + Bech32.separator ("1") + encoded data ("w0rld") + 6 char checksum ("jn365x")

    // free memory
    bech32_free_bstring(bstring);

    return E_BECH32_SUCCESS;
}
