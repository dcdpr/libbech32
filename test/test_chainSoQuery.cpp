#include <gtest/gtest.h>
#include <utility>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#include <json/json.h>
#pragma clang diagnostic pop

#include "chainQuery.cpp"
#include "chainSoQuery.cpp"
#include "curlWrapper.cpp"
#include "jsonTestData.h"

/**
 * An "injectable" version of ChainSoQuery. We can create instances of this class that will return
 * specific examples of JSON data so we can unit test the member functions of the ChainSoQuery
 * class without having to hit the network.
 */
class Injectable_ChainSoQuery : public ChainSoQuery {

private:
    std::string jsonData;

public:

    virtual ~Injectable_ChainSoQuery() override;

    explicit Injectable_ChainSoQuery(const std::string & data)
        : jsonData(data)
    {}

    // public wrapper method so we can test protected method
    int wrapDetermineNextUtxoIndex(
            const std::string & nextTxid,
            const std::string & network) const
    {
        return determineNextUtxoIndex(nextTxid, network);
    }

    // public wrapper method so we can test protected method
    std::string wrapExtractLastUpdatedTxid(
            const Json::Value &obj,
            const std::string &txid,
            const std::string &network) const
    {
        return extractLastUpdatedTxid(obj, txid, network);
    }

protected:

    // override so we don't talk to network, but instead return test data string
    std::string retrieveJsonData(const std::string & , int) const override {
        return jsonData;
    }

};

Injectable_ChainSoQuery::~Injectable_ChainSoQuery() = default;


TEST(ChainSoQueryTest, fail_to_get_index_zero) {

    Injectable_ChainSoQuery q(get_tx_unspent_data_fail);

    EXPECT_THROW(q.getUnspentOutputs("mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy", 0, "test"), std::runtime_error);
}

TEST(ChainSoQueryTest, get_index_zero) {

    Injectable_ChainSoQuery q(get_tx_unspent_data_success);
    UnspentData d;
    d = q.getUnspentOutputs("mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy", 0, "test");

    EXPECT_EQ(d.txid, "2ee994e55b8c71a6f4025a04ebec08aeaffc42c914a9a3646cb3e35489d0e705");
    EXPECT_EQ(d.amountSatoshis, 16250000);
}

TEST(ChainSoQueryTest, get_index_one) {

    Injectable_ChainSoQuery q(get_tx_unspent_data_success);
    UnspentData d;
    d = q.getUnspentOutputs("mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy", 1, "test");

    EXPECT_EQ(d.txid, "2ee994e55b8c71a6f4025a04ebec08aeaffc42c914a9a3646cb3e35489d0e706");
    EXPECT_EQ(d.amountSatoshis, 18250000);

}

TEST(ChainSoQueryTest, fail_to_get_index_two) {

    Injectable_ChainSoQuery q(get_tx_unspent_data_success);

    EXPECT_THROW(q.getUnspentOutputs("mr85paFJtFCJyHfyHPeaftU4ytNvNGRSwy", 2, "test"), std::runtime_error);
}

TEST(ChainSoQueryTest, determine_next_txo_index_0) {

    Injectable_ChainSoQuery q(get_tx_data_opreturn_at_1);

    int i = q.wrapDetermineNextUtxoIndex("e6b8eae524742c6d87f95d8037cbba2978db4b65a9005adce31c912c195d70e9", "test");

    EXPECT_EQ(i, 0);
}

TEST(ChainSoQueryTest, determine_next_txo_index_1) {

    Injectable_ChainSoQuery q(get_tx_data_opreturn_at_0);

    int i = q.wrapDetermineNextUtxoIndex("e6b8eae524742c6d87f95d8037cbba2978db4b65a9005adce31c912c195d70e9", "test");

    EXPECT_EQ(i, 1);
}

TEST(ChainSoQueryTest, fail_to_get_spent_data) {

    Injectable_ChainSoQuery q(is_spent_data_fail);

    EXPECT_THROW(q.getLastUpdatedTxid("dummy", 0, "test"), std::runtime_error);
}

TEST(ChainSoQueryTest, get_spent_data_not_spent) {

    Injectable_ChainSoQuery q(is_spent_data_false);

    std::string firstTxid = "e6b8eae524742c6d87f95d8037cbba2978db4b65a9005adce31c912c195d70e9";
    std::string nextTxid = q.getLastUpdatedTxid(firstTxid, 1, "test");
    EXPECT_EQ(nextTxid, firstTxid);
}


