// test program calling bech32 library from C++

#include "libbech32.h"
#include <stdexcept>

// make sure we can run these tests even when building a release version
#undef NDEBUG
#include <cassert>


void stripUnknownChars_withSimpleString_returnsSameString() {
    std::string src = "ffff";
    assert(src == bech32::stripUnknownChars(src));
}

void stripUnknownChars_withComplexString_returnsStrippedString() {
    std::string src = "foobar";
    std::string expected = "far";
    assert(expected == bech32::stripUnknownChars(src));
}

void stripUnknownChars_withFunkyString_returnsStrippedString() {
    std::string src = "tx1!rjk0\\u5ng*4jsf^^mc";
    std::string expected = "tx1rjk0u5ng4jsfmc";
    assert(expected == bech32::stripUnknownChars(src));
}

// ----- tests using default checksum constant = M (0x2bc830a3) ------

void decode_minimalExample_isSuccessful() {
    std::string bstr = "a1lqfn3a";
    std::string expectedHrp = "a";

    bech32::HrpAndDp hd = bech32::decode(bstr);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32m == hd.encoding);
}

void decode_longExample_isSuccessful() {
    std::string bstr = "abcdef1l7aum6echk45nj3s0wdvt2fg8x9yrzpqzd3ryx";
    std::string expectedHrp = "abcdef";

    bech32::HrpAndDp hd = bech32::decode(bstr);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32m == hd.encoding);

    assert(hd.dp[0] == '\x1f'); // first 'l' in above dp part
    assert(hd.dp[31] == '\0');  // last 'q' in above dp part
}

void decode_minimalExampleBadChecksum_isUnsuccessful() {
    std::string bstr = "a1lqfn3q"; // last 'q' should be a 'a'
    std::string expectedHrp = "a";

    bech32::HrpAndDp hd = bech32::decode(bstr);

    assert(hd.hrp.empty());
    assert(hd.dp.empty());
}

void decode_whenMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to decode a string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    std::string bstr = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    std::string expectedHrp = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";

    try {
        bech32::HrpAndDp hd = bech32::decode(bstr);
    }
    catch (std::runtime_error &e) {
        assert(std::string(e.what()) == "bech32 string too long");
    }
}

void encode_emptyExample_isUnsuccessful() {
    std::string hrp;
    std::vector<unsigned char> dp = {};
    std::string expected = "a1lqfn3a";

    try {
        bech32::encode(hrp, dp);
    }
    catch (std::runtime_error &e) {
        assert(std::string(e.what()) == "HRP must be at least one character");
    }
}

void encode_minimalExample_isSuccessful() {
    std::string hrp = "a";
    std::vector<unsigned char> dp = {};
    std::string expected = "a1lqfn3a";

    assert(expected == bech32::encode(hrp, dp));
}

void encode_smallExample_isSuccessful() {
    std::string hrp = "xyz";
    std::vector<unsigned char> dp = {1,2,3};
    std::string expected = "xyz1pzrs3usye";

    assert(expected == bech32::encode(hrp, dp));
}

void encode_whenMethodThrowsException_isUnsuccessful() {
    // bech32 string can only have HRPs that are 83 chars or less. Attempt to encode an HRP string
    // with more than 83 chars and make sure that the exception thrown in the C++ code is caught
    // and returns an error code
    std::string hrp = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    std::vector<unsigned char> dp = {1,2,3};

    try {
        bech32::encode(hrp, dp);
    }
    catch (std::runtime_error &e) {
        assert(std::string(e.what()) == "HRP must be less than 84 characters");
    }
}

void decode_and_encode_minimalExample_producesSameResult() {
    std::string bstr1 = "a1lqfn3a";
    std::string expectedHrp = "a";

    bech32::HrpAndDp hd = bech32::decode(bstr1);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32m == hd.encoding);

    std::string bstr2 = bech32::encode(hd.hrp, hd.dp);

    assert(bstr1 == bstr2);
}

void decode_and_encode_smallExample_producesSameResult() {
    std::string bstr1 = "xyz1pzrs3usye";
    std::string expectedHrp = "xyz";

    bech32::HrpAndDp hd = bech32::decode(bstr1);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32m == hd.encoding);

    std::string bstr2 = bech32::encode(hd.hrp, hd.dp);

    assert(bstr1 == bstr2);
}

void decode_and_encode_longExample_producesSameResult() {
    std::string bstr1 = "abcdef1l7aum6echk45nj3s0wdvt2fg8x9yrzpqzd3ryx";
    std::string expectedHrp = "abcdef";

    bech32::HrpAndDp hd = bech32::decode(bstr1);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32m == hd.encoding);

    std::string bstr2 = bech32::encode(hd.hrp, hd.dp);

    assert(bstr1 == bstr2);
}

// ---------- tests using original checksum constant = 1 ------------

void decode_c1_minimalExample_isSuccessful() {
    std::string bstr = "a12uel5l";
    std::string expectedHrp = "a";

    bech32::HrpAndDp hd = bech32::decode(bstr);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32 == hd.encoding);
}

void decode_c1_longExample_isSuccessful() {
    std::string bstr = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    std::string expectedHrp = "abcdef";

    bech32::HrpAndDp hd = bech32::decode(bstr);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32 == hd.encoding);

    assert(hd.dp[0] == '\0');    // first 'q' in above dp part
    assert(hd.dp[31] == '\x1f'); // last 'l' in above dp part
}

void encode_c1_minimalExample_isSuccessful() {
    std::string hrp = "a";
    std::vector<unsigned char> dp = {};
    std::string expected = "a12uel5l";

    assert(expected == bech32::encode_bech32_1(hrp, dp));
}

void encode_c1_smallExample_isSuccessful() {
    std::string hrp = "xyz";
    std::vector<unsigned char> dp = {1,2,3};
    std::string expected = "xyz1pzr9dvupm";

    assert(expected == bech32::encode_bech32_1(hrp, dp));
}

void decode_and_encode_c1_minimalExample_producesSameResult() {
    std::string bstr1 = "a12uel5l";
    std::string expectedHrp = "a";

    bech32::HrpAndDp hd = bech32::decode(bstr1);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32 == hd.encoding);

    std::string bstr2 = bech32::encode_bech32_1(hd.hrp, hd.dp);

    assert(bstr1 == bstr2);
}

void decode_and_encode_c1_smallExample_producesSameResult() {
    std::string bstr1 = "xyz1pzr9dvupm";
    std::string expectedHrp = "xyz";

    bech32::HrpAndDp hd = bech32::decode(bstr1);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32 == hd.encoding);

    std::string bstr2 = bech32::encode_bech32_1(hd.hrp, hd.dp);

    assert(bstr1 == bstr2);
}

void decode_and_encode_c1_longExample_producesSameResult() {
    std::string bstr1 = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lmqqqxw";
    std::string expectedHrp = "abcdef";

    bech32::HrpAndDp hd = bech32::decode(bstr1);

    assert(expectedHrp == hd.hrp);
    assert(bech32::Encoding::Bech32 == hd.encoding);

    std::string bstr2 = bech32::encode_bech32_1(hd.hrp, hd.dp);

    assert(bstr1 == bstr2);
}


void tests_using_default_checksum_constant() {
    stripUnknownChars_withSimpleString_returnsSameString();
    stripUnknownChars_withComplexString_returnsStrippedString();
    stripUnknownChars_withFunkyString_returnsStrippedString();

    decode_whenMethodThrowsException_isUnsuccessful();
    decode_minimalExample_isSuccessful();
    decode_longExample_isSuccessful();
    decode_minimalExampleBadChecksum_isUnsuccessful();

    encode_whenMethodThrowsException_isUnsuccessful();
    encode_emptyExample_isUnsuccessful();
    encode_minimalExample_isSuccessful();
    encode_smallExample_isSuccessful();

    decode_and_encode_minimalExample_producesSameResult();
    decode_and_encode_smallExample_producesSameResult();
    decode_and_encode_longExample_producesSameResult();
}

void tests_using_original_checksum_constant() {
    decode_c1_minimalExample_isSuccessful();
    decode_c1_longExample_isSuccessful();

    encode_c1_minimalExample_isSuccessful();
    encode_c1_smallExample_isSuccessful();

    decode_and_encode_c1_minimalExample_producesSameResult();
    decode_and_encode_c1_smallExample_producesSameResult();
    decode_and_encode_c1_longExample_producesSameResult();
}

int main() {

    tests_using_default_checksum_constant();
    tests_using_original_checksum_constant();

    return 0;
}


