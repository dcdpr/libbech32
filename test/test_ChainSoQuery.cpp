#include <gtest/gtest.h>
#include <json/json.h>

#include "chainSoQuery.cpp"
#include "CurlWrapper.cpp"

class FakeChainSoQuery : public ChainSoQuery {

protected:
    std::string retrieveJsonData(const std::string & url) const override {
        std::string result = ""
                "{\n"
                "  \"status\" : \"success\",\n"
                "  \"data\" : {\n"
                "    \"network\" : \"BTCTEST\",\n"
                "    \"address\" : \"mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy\",\n"
                "    \"txs\" : [\n"
                "      {\n"
                "        \"txid\" : \"2ee994e55b8c71a6f4025a04ebec08aeaffc42c914a9a3646cb3e35489d0e705\",\n"
                "        \"output_no\" : 0,\n"
                "        \"script_asm\" : \"OP_DUP OP_HASH160 7456195c1c326236f3a5bc17bdd868ce45ccdad8 OP_EQUALVERIFY OP_CHECKSIG\",\n"
                "        \"script_hex\" : \"76a9147456195c1c326236f3a5bc17bdd868ce45ccdad888ac\",\n"
                "        \"value\" : \"0.16250000\",\n"
                "        \"confirmations\" : 1924,\n"
                "        \"time\" : 1520470152\n"
                "      },\n"
                "      {\n"
                "        \"txid\" : \"2ee994e55b8c71a6f4025a04ebec08aeaffc42c914a9a3646cb3e35489d0e705\",\n"
                "        \"output_no\" : 1,\n"
                "        \"script_asm\" : \"OP_DUP OP_HASH160 7456195c1c326236f3a5bc17bdd868ce45ccdad8 OP_EQUALVERIFY OP_CHECKSIG\",\n"
                "        \"script_hex\" : \"76a9147456195c1c326236f3a5bc17bdd868ce45ccdad888ac\",\n"
                "        \"value\" : \"0.16250000\",\n"
                "        \"confirmations\" : 1924,\n"
                "        \"time\" : 1520470152\n"
                "      }\n"
                "    ]\n"
                "  }\n"
                "}";
        return result;
    }

};


TEST(ChainSoQueryTest, get_index_zero) {

    FakeChainSoQuery q;
    UnspentData d;
    d = q.getUnspentOutputs("mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy", 0, "test");

    EXPECT_EQ(d.txid, "2ee994e55b8c71a6f4025a04ebec08aeaffc42c914a9a3646cb3e35489d0e705");
    EXPECT_EQ(d.amountSatoshis, 16250000);
}

TEST(ChainSoQueryTest, get_index_one) {

    FakeChainSoQuery q;
    UnspentData d;
    d = q.getUnspentOutputs("mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy", 1, "test");

    EXPECT_EQ(d.txid, "2ee994e55b8c71a6f4025a04ebec08aeaffc42c914a9a3646cb3e35489d0e705");
    EXPECT_EQ(d.amountSatoshis, 16250000);

}

TEST(ChainSoQueryTest, fail_to_get_index_two) {

    FakeChainSoQuery q;
    UnspentData d;

    EXPECT_THROW(d = q.getUnspentOutputs("mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy", 2, "test"), std::runtime_error);
}

// TODO this test will fail until I create a new test address with unspent outputs
//TEST(ChainSoQueryTest, get_index_zero_integration) {
//
//    chainSoQuery q;
//    UnspentData d;
//    d = q.getUnspentOutputs("mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy", 0, "test");
//
//    EXPECT_EQ(d.txid, "2ee994e55b8c71a6f4025a04ebec08aeaffc42c914a9a3646cb3e35489d0e705");
//    EXPECT_EQ(d.amountSatoshis, 16250000);
//}

