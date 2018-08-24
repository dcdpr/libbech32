#include <iostream>
#include <unistd.h>
#include <cassert>
#include "chainSoQuery.h"

void testGetUnspentOutputs() {

}

void testGetLastUpdatedTxid() {
    ChainSoQuery q;

    const std::string refTxid = "8a76b282fa1e3585d5c4c0dd2774400aa0a075e2cd255f0f5324f2e837f282c5";

    std::string txid, lastTxid;

    // the first txid starts a chain of four transactions:
    // e6b8... -> cb02... -> 3289... -> 8a76...
    // asking the query object for the "last updated txid" for each txid in
    // the chain should produce the same last txid (8a76...)

    txid = "e6b8eae524742c6d87f95d8037cbba2978db4b65a9005adce31c912c195d70e9";
    lastTxid = q.getLastUpdatedTxid(txid, 1, "test");
    assert(lastTxid == refTxid);

    txid = "cb0252c5ea4e24bee19edd1ed1338ef077dc75d30383097d8c4bae3a9862b35a";
    lastTxid = q.getLastUpdatedTxid(txid, 1, "test");
    assert(lastTxid == refTxid);

    txid = "3289a36a8ca15017269a6b6e3977732fc21c8106d85020d5493ee9f5c8e00958";
    lastTxid = q.getLastUpdatedTxid(txid, 1, "test");
    assert(lastTxid == refTxid);
}

int main(int, char **) {

    testGetUnspentOutputs();
    testGetLastUpdatedTxid();

    return 0;
}

