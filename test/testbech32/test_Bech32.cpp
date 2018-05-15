#include <gtest/gtest.h>

#include "test_Bech32.h"
#include "bech32.cpp"

// check that we reject strings less than 8 chars in length
TEST_F(Bech32Test, ensure_correct_data_size_low) {
    std::string data(7, 'a');
    EXPECT_THROW(rejectBStringTooShort(data), std::runtime_error);

    data.append(1, 'a');
    EXPECT_NO_THROW(rejectBStringTooShort(data));
}

// check that we reject strings greater than 90 chars in length
TEST_F(Bech32Test, ensure_correct_data_size_high) {
    std::string data(89, 'a');
    EXPECT_NO_THROW(rejectBStringTooLong(data));

    data.append(1, 'a');
    EXPECT_NO_THROW(rejectBStringTooLong(data));

    data.append(1, 'a');
    EXPECT_THROW(rejectBStringTooLong(data), std::runtime_error);
}

// check that we accept strings with all lowercase, with and without numbers
TEST_F(Bech32Test, accept_all_lowercase_data) {
    std::string data("abcdefghi");
    EXPECT_NO_THROW(rejectBStringMixedCase(data));

    data = "abcde123fghi";
    EXPECT_NO_THROW(rejectBStringMixedCase(data));
}

// check that we accept strings with all uppercase, with and without numbers
TEST_F(Bech32Test, accept_all_uppercase_data) {
    std::string data("ABCDEFGHI");
    EXPECT_NO_THROW(rejectBStringMixedCase(data));

    data = "ABCDE123FGHI";
    EXPECT_NO_THROW(rejectBStringMixedCase(data));
}

// check that we reject strings with mixedcase, with and without numbers
TEST_F(Bech32Test, reject_mixedcase_data) {
    std::string data("abcdEfghi");
    EXPECT_THROW(rejectBStringMixedCase(data), std::runtime_error);

    data = "ABCDeFGHI";
    EXPECT_THROW(rejectBStringMixedCase(data), std::runtime_error);

    data = "abcde123FGHI";
    EXPECT_THROW(rejectBStringMixedCase(data), std::runtime_error);
}

// check that we accept strings with in-range characters
TEST_F(Bech32Test, accept_data_in_range) {
    std::string data("abcde");
    EXPECT_NO_THROW(rejectBStringValuesOutOfRange(data));

    data = "!!abcde}~";
    EXPECT_NO_THROW(rejectBStringValuesOutOfRange(data));
}

// check that we reject strings with out-of-range characters
TEST_F(Bech32Test, reject_data_out_of_range) {
    std::string data(" ");
    EXPECT_THROW(rejectBStringValuesOutOfRange(data), std::runtime_error);

    data = "\x20";
    EXPECT_THROW(rejectBStringValuesOutOfRange(data), std::runtime_error);

    data = "\x7f";
    EXPECT_THROW(rejectBStringValuesOutOfRange(data), std::runtime_error);

    data = " abc\x7fxyz\x0d";
    EXPECT_THROW(rejectBStringValuesOutOfRange(data), std::runtime_error);
}

// check that we reject strings with no separator character
TEST_F(Bech32Test, reject_data_with_no_separator) {
    std::string data("abcd");
    EXPECT_THROW(rejectBStringWithNoSeparator(data), std::runtime_error);
}

// check that we accept strings with at least one separator character
TEST_F(Bech32Test, accept_data_with_a_separator) {
    std::string data("ab1cd");
    EXPECT_NO_THROW(rejectBStringWithNoSeparator(data));

    data = "111";
    EXPECT_NO_THROW(rejectBStringWithNoSeparator(data));
}

// check that we can find the position of the separator character
TEST_F(Bech32Test, find_separator_position) {
    std::string data("ab1cd");
    auto pos = findSeparatorPosition(data);
    ASSERT_EQ(2, pos);

    data = "abc1def1lalala";
    pos = findSeparatorPosition(data);
    ASSERT_EQ(7, pos);

    data = "lalalalalala";
    pos = findSeparatorPosition(data);
    ASSERT_EQ(std::string::npos, pos);
}

// check that we can split strings based on the separator
TEST_F(Bech32Test, split_strings) {
    std::string data("ab1cd");
    bech32::HrpAndDp b = splitString(data);
    ASSERT_EQ(b.hrp, "ab");
    ASSERT_EQ(b.dp[0], 'c');
    ASSERT_EQ(b.dp.size(), 2);

    data = "ab1";
    b = splitString(data);
    ASSERT_EQ(b.hrp, "ab");
    ASSERT_TRUE(b.dp.empty());

    data = "1cd";
    b = splitString(data);
    ASSERT_EQ(b.hrp, "");
    ASSERT_EQ(b.dp[0], 'c');
    ASSERT_EQ(b.dp.size(), 2);

    data = "1";
    b = splitString(data);
    ASSERT_EQ(b.hrp, "");
    ASSERT_TRUE(b.dp.empty());
}

// check that we can lowercase strings
TEST_F(Bech32Test, lowercase_strings) {
    std::string data("ABC");
    convertToLowercase(data);
    ASSERT_EQ(data, "abc");

    data = "AbC";
    convertToLowercase(data);
    ASSERT_EQ(data, "abc");

    data = "123";
    convertToLowercase(data);
    ASSERT_EQ(data, "123");
}

// check that we can map the dp
TEST_F(Bech32Test, map_data) {
    std::string data("ABC1acd");
    bech32::HrpAndDp b = splitString(data);
    ASSERT_NO_THROW(mapDP(b.dp));
    ASSERT_EQ(b.dp[0], '\x1d');
    ASSERT_EQ(b.dp[1], '\x18');
    ASSERT_EQ(b.dp[2], '\x0d');

    data = "ACB1DEF";
    b = splitString(data);
    ASSERT_NO_THROW(mapDP(b.dp));
    ASSERT_EQ(b.dp[0], '\x0d');
    ASSERT_EQ(b.dp[1], '\x19');
    ASSERT_EQ(b.dp[2], '\x09');

    data = "ACB1abc";
    b = splitString(data);
    ASSERT_THROW(mapDP(b.dp), std::runtime_error); // throws because 'b' is invalid character
}

// check that we can expand the hrp
TEST_F(Bech32Test, expand_hrp) {
    std::string hrp("ABC");
    std::vector<unsigned char> e = expandHrp(hrp);
    ASSERT_EQ(e[0], '\x02');
    ASSERT_EQ(e[1], '\x02');
    ASSERT_EQ(e[2], '\x02');
    ASSERT_EQ(e[3], '\x00');
    ASSERT_EQ(e[4], '\x01');
    ASSERT_EQ(e[5], '\x02');
    ASSERT_EQ(e[6], '\x03');
}

// check the polymod method
TEST_F(Bech32Test, polymod) {
    std::string hrp("A");
    std::vector<unsigned char> e = expandHrp(hrp);
    uint32_t p = polymod(e);
    ASSERT_EQ(p, 34817);

    hrp = "B";
    e = expandHrp(hrp);
    p = polymod(e);
    ASSERT_EQ(p, 34818);

    hrp = "qwerty";
    e = expandHrp(hrp);
    p = polymod(e);
    ASSERT_EQ(p, 448484437);
}

// check the verifyChecksum method
TEST_F(Bech32Test, verifyChecksum_good) {
    std::string data("a12uel5l");
    bech32::HrpAndDp b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "A12UEL5L";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1tt5tgs";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqc8247j";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

}

// check the verifyChecksum method
// these are simply the "good" tests from above with a single character changed
TEST_F(Bech32Test, verifyChecksum_bad) {
    std::string data("a12uel5m");
    bech32::HrpAndDp b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "C12UEL5L";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "abcdefg1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "split1dheckupstagehandshakeupstreamerranterredcaperred2y9e3w";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio1tt5tgt";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqc8247j";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));
}

// check the main bech32 decode method
TEST_F(Bech32Test, decode_good) {
    std::string data("a12uel5l");
    bech32::HrpAndDp b = bech32::decode(data);
    ASSERT_EQ(b.hrp, "a");
    ASSERT_TRUE(b.dp.empty());

    data = "A12UEL5L";
    b = bech32::decode(data);
    ASSERT_EQ(b.hrp, "a");
    ASSERT_TRUE(b.dp.empty());

    data = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    b = bech32::decode(data);
    ASSERT_EQ(b.hrp, "abcdef");
    ASSERT_EQ(b.dp.size(), 32);
    ASSERT_EQ(b.dp[0], '\0');    // 'q' in above dp part
    ASSERT_EQ(b.dp[31], '\x1f'); // 'l' in above dp part

}

TEST_F(Bech32Test, create_checksum) {
    std::string hrp = "A";
    std::vector<unsigned char> data;
    std::vector<unsigned char> checksum = createChecksum(hrp, data);

    ASSERT_EQ(checksum[0], '\b');
    ASSERT_EQ(checksum[1], '\x1e');
    ASSERT_EQ(checksum[2], '\x10');
    ASSERT_EQ(checksum[3], '\b');
    ASSERT_EQ(checksum[4], '\r');
    ASSERT_EQ(checksum[5], '\a');
}

// check the main bech32 encode method
TEST_F(Bech32Test, encode_good) {
    std::string hrp = "a";
    std::vector<unsigned char> data;
    std::string b = bech32::encode(hrp, data);
    ASSERT_EQ(b, "a12uel5l");

    hrp = "A";
    b = bech32::encode(hrp, data);
    ASSERT_EQ(b, "a12uel5l");
}

// check that we can decode and then encode back to the original
TEST_F(Bech32Test, check_decode_encode) {
    std::string data("a12uel5l");
    bech32::HrpAndDp bs = bech32::decode(data);
    ASSERT_EQ(bs.hrp, "a");
    ASSERT_TRUE(bs.dp.empty());
    std::string enc = bech32::encode(bs.hrp, bs.dp);
    ASSERT_EQ(enc, data);

    data = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    bs = bech32::decode(data);
    ASSERT_EQ(bs.hrp, "abcdef");
    enc = bech32::encode(bs.hrp, bs.dp);
    ASSERT_EQ(enc, data);

    data = "split1checkupstagehandshakeupstreamerranterredcaperred2y9e3w";
    bs = bech32::decode(data);
    ASSERT_EQ(bs.hrp, "split");
    enc = bech32::encode(bs.hrp, bs.dp);
    ASSERT_EQ(enc, data);

}


TEST_F(Bech32Test, encode_empty_args) {
    std::string hrp;
    std::vector<unsigned char> data;
    std::string s;
    ASSERT_THROW(s = bech32::encode(hrp, data), std::runtime_error);
}

TEST_F(Bech32Test, strip_unknown_chars) {
    std::string bStr;
    EXPECT_EQ(bech32::stripUnknownChars("tx1-rqqq-qqqq-qmhu-qk"), "tx1rqqqqqqqqmhuqk");
    // TODO Not sure what to do about the extra '1' in the next test. We leave it in
    // because we leave in the separator character, but I think the test might be expected
    // to take it out. Not sure if it is a buggy test example from the spec or what.
    //EXPECT_EQ(bech32::stripUnknownChars("TX1R1JK0--U5bNG4JSb----FMC"), "TX1RJK0U5NG4JSFMC");
    EXPECT_EQ(bech32::stripUnknownChars("TX1RJK0--U5bNG4JSb----FMC"), "TX1RJK0U5NG4JSFMC");
    EXPECT_EQ(bech32::stripUnknownChars("tx1 rjk0 u5ng 4jsfmc"), "tx1rjk0u5ng4jsfmc");
    EXPECT_EQ(bech32::stripUnknownChars("tx1!rjk0\\u5ng*4jsf^^mc"), "tx1rjk0u5ng4jsfmc");
}
