#include <gtest/gtest.h>
#include <json/json.h>

#include "encodeOpReturnData.h"


TEST(EncodeOpReturnDataTest, test_simple) {
    std::string encoded = encodeOpReturnData("a");
    EXPECT_EQ(encoded, "61");
}

TEST(EncodeOpReturnDataTest, test_hello) {
    std::string encoded = encodeOpReturnData("hello world");
    EXPECT_EQ(encoded, "68656c6c6f20776f726c64");
}

TEST(EncodeOpReturnDataTest, test_url) {
    std::string encoded = encodeOpReturnData("https://www.example.com/some/path/ddo.jsonld");
    EXPECT_EQ(encoded, "68747470733a2f2f7777772e6578616d706c652e636f6d2f736f6d652f706174682f64646f2e6a736f6e6c64");
}


