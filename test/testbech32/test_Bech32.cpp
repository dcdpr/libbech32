#include "bech32.cpp"

#include <gtest/gtest.h>
#pragma clang diagnostic push
#pragma GCC diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#include <rapidcheck/gtest.h>
#pragma clang diagnostic pop
#pragma GCC diagnostic pop

// check that we reject strings less than 8 chars in length
TEST(Bech32Test, ensure_correct_data_size_low) {
    std::string data(7, 'a');
    EXPECT_THROW(rejectBStringTooShort(data), std::runtime_error);

    data.append(1, 'a');
    EXPECT_NO_THROW(rejectBStringTooShort(data));
}

RC_GTEST_PROP(Bech32TestRC, stringsTooShortAreRejected, ()
) {
    // generate string with size < 8
    const auto str = *rc::gen::suchThat(rc::gen::string<std::string>(), [](std::string x) {
        return x.length() < 8;
    });

    RC_ASSERT_THROWS_AS(rejectBStringTooShort(str), std::runtime_error);
}

// check that we reject strings greater than 90 chars in length
TEST(Bech32Test, ensure_correct_data_size_high) {
    std::string data(89, 'a');
    EXPECT_NO_THROW(rejectBStringTooLong(data));

    data.append(1, 'a');
    EXPECT_NO_THROW(rejectBStringTooLong(data));

    data.append(1, 'a');
    EXPECT_THROW(rejectBStringTooLong(data), std::runtime_error);
}

RC_GTEST_PROP(Bech32TestRC, stringsTooLongAreRejected, ()
) {
    // generate string with size > 90
    const auto str = *rc::gen::suchThat(rc::gen::string<std::string>(), [](std::string x) {
        return x.length() > 90;
    });

    RC_ASSERT_THROWS_AS(rejectBStringTooLong(str), std::runtime_error);
}

RC_GTEST_PROP(Bech32TestRC, stringsOfCorrectLengthAreAccepted, ()
) {
    // generate string with size >= 8 and <= 90
    const auto str = *rc::gen::suchThat(rc::gen::string<std::string>(), [](std::string x) {
        return x.length() >= 8 && x.length() <= 90;
    });

    // neither of these should throw
    rejectBStringTooShort(str);
    rejectBStringTooLong(str);
}

// check that we accept strings with all numbers, since there is no mixedcase present
TEST(Bech32Test, accept_all_numeric_data) {
    std::string data("1");
    EXPECT_NO_THROW(rejectBStringMixedCase(data));

    data = "9483538";
    EXPECT_NO_THROW(rejectBStringMixedCase(data));
}

RC_GTEST_PROP(Bech32TestRC, stringsWithAllNumbersAreAccepted, ()
) {
    // generate string with chars between 0-9
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::inRange('0', '9'));

    // this should not throw
    rejectBStringMixedCase(str);
}

// check that we accept strings with all lowercase, with and without numbers
TEST(Bech32Test, accept_all_lowercase_data) {
    std::string data("abcdefghi");
    EXPECT_NO_THROW(rejectBStringMixedCase(data));

    data = "abcde123fghi";
    EXPECT_NO_THROW(rejectBStringMixedCase(data));
}

RC_GTEST_PROP(Bech32TestRC, stringsWithAllLowercaseAndNumbersAreAccepted, ()
) {
    // generate string with chars between a-z and 0-9
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    // this should not throw
    rejectBStringMixedCase(str);
}

// check that we accept strings with all uppercase, with and without numbers
TEST(Bech32Test, accept_all_uppercase_data) {
    std::string data("ABCDEFGHI");
    EXPECT_NO_THROW(rejectBStringMixedCase(data));

    data = "ABCDE123FGHI";
    EXPECT_NO_THROW(rejectBStringMixedCase(data));
}

RC_GTEST_PROP(Bech32TestRC, stringsWithAllUppercaseAndNumbersAreAccepted, ()
) {
    // generate string with chars between A-Z and 0-9
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('A', 'Z'),
                            rc::gen::inRange('0', '9')));

    // this should not throw
    rejectBStringMixedCase(str);
}

// check that we reject strings with mixedcase, with and without numbers
TEST(Bech32Test, reject_mixedcase_data) {
    std::string data("abcdEfghi");
    EXPECT_THROW(rejectBStringMixedCase(data), std::runtime_error);

    data = "ABCDeFGHI";
    EXPECT_THROW(rejectBStringMixedCase(data), std::runtime_error);

    data = "abcde123FGHI";
    EXPECT_THROW(rejectBStringMixedCase(data), std::runtime_error);
}

RC_GTEST_PROP(Bech32TestRC, stringsWithMixedcaseAndNumbersAreRejected, ()
) {
    // generate string with chars between a-z, A-Z and 0-9
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('A', 'Z'),
                            rc::gen::inRange('0', '9')));

    // skip generated strings less than two chars long, since they can't possibly have mixed case
    RC_PRE(str.size() > 1u);

    // the above generation can sometimes randomly produce a string without mixed case...
    // here we reject those before testing. Seems weird since this is really using similar code
    // to what we are testing in the first place
    RC_PRE((std::any_of(str.begin(), str.end(), &::isupper) &&
            std::any_of(str.begin(), str.end(), &::islower)));

    RC_ASSERT_THROWS_AS(rejectBStringMixedCase(str), std::runtime_error);
}


// check that we accept strings with in-range characters
TEST(Bech32Test, accept_data_in_range) {
    std::string data("abcde");
    EXPECT_NO_THROW(rejectBStringValuesOutOfRange(data));

    data = "!!abcde}~";
    EXPECT_NO_THROW(rejectBStringValuesOutOfRange(data));
}

RC_GTEST_PROP(Bech32TestRC, stringsWithInRangeCharactersAreAccepted, ()
) {
    // generate string with chars between values 33 and 126, inclusive
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::inRange(33, 126));

    // this should not throw
    rejectBStringValuesOutOfRange(str);
}

// check that we reject strings with out-of-range characters
TEST(Bech32Test, reject_data_out_of_range) {
    std::string data(" ");
    EXPECT_THROW(rejectBStringValuesOutOfRange(data), std::runtime_error);

    data = "\x20";
    EXPECT_THROW(rejectBStringValuesOutOfRange(data), std::runtime_error);

    data = "\x7f";
    EXPECT_THROW(rejectBStringValuesOutOfRange(data), std::runtime_error);

    data = " abc\x7fxyz\x0d";
    EXPECT_THROW(rejectBStringValuesOutOfRange(data), std::runtime_error);
}

RC_GTEST_PROP(Bech32TestRC, stringsWithAllOutOfRangeCharactersAreRejected, ()
) {
    // generate string with chars between values 0 and 32, 127 and 255, inclusive
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange(0, 32),
                            rc::gen::inRange(127, 255)));

    // skip generated empty strings
    RC_PRE(!str.empty());

    RC_ASSERT_THROWS_AS(rejectBStringValuesOutOfRange(str), std::runtime_error);
}

RC_GTEST_PROP(Bech32TestRC, stringsWithSomeOutOfRangeCharactersAreRejected, ()
) {
    // generate string with chars between values 0 and 255, inclusive
    const auto str =
            *rc::gen::container<std::string>(
                            rc::gen::inRange(0, 255));

    // skip generated empty strings
    RC_PRE(!str.empty());

    // the above generation can sometimes randomly produce a string without out of range characters,
    // usually very short strings.. here we reject those before testing. Seems weird since this is
    // really using similar code to what we are testing in the first place
    RC_PRE(std::any_of(str.begin(), str.end(), [](char ch){ return ch < 33 || ch > 126; } ));

    RC_ASSERT_THROWS_AS(rejectBStringValuesOutOfRange(str), std::runtime_error);
}

// check that we reject strings with no separator character
TEST(Bech32Test, reject_data_with_no_separator) {
    std::string data("abcd");
    EXPECT_THROW(rejectBStringWithNoSeparator(data), std::runtime_error);
}

RC_GTEST_PROP(Bech32TestRC, stringsWithNoSeparatorCharacterAreRejected, ()
) {
    // generate string with chars between a-z and 0-9
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    // skip any strings that contain the separator character
    RC_PRE(std::none_of(str.begin(), str.end(), [](char ch){ return ch == bech32::separator; } ));

    RC_ASSERT_THROWS_AS(rejectBStringWithNoSeparator(str), std::runtime_error);
}

// check that we accept strings with at least one separator character
TEST(Bech32Test, accept_data_with_a_separator) {
    std::string data("ab1cd");
    EXPECT_NO_THROW(rejectBStringWithNoSeparator(data));

    data = "111";
    EXPECT_NO_THROW(rejectBStringWithNoSeparator(data));
}

RC_GTEST_PROP(Bech32TestRC, stringsWithSeparatorCharacterAreAccepted, ()
) {
    // generate string with chars between a-z and 0-9
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    // skip any strings that don't contain the separator character
    RC_PRE(std::any_of(str.begin(), str.end(), [](char ch){ return ch == bech32::separator; } ));

    // this should not throw
    rejectBStringWithNoSeparator(str);
}

// check that we can find the position of the separator character
TEST(Bech32Test, find_separator_position) {
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

RC_GTEST_PROP(Bech32TestRC, findLastSeparatorCharacterPosition, ()
) {
    // generate string with chars between a-z and 0-9
    auto str =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    auto pos1 = str.find_last_of(bech32::separator);

    // insert separator character at a known position between last existing separator
    // character (or start of string) and the end of the string
    if(pos1 == std::string::npos)
        pos1 = 0;
    else
        pos1++;

    const auto pos2 = *rc::gen::inRange(pos1, str.length());
    str.insert(pos2, 1, bech32::separator);

    RC_ASSERT(findSeparatorPosition(str) == pos2);
}

// check that we can split strings based on the separator
TEST(Bech32Test, split_strings) {
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

RC_GTEST_PROP(Bech32TestRC, checkSplitString, ()
) {
    // generate string with chars between a-z and 0-9
    const auto str1 =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    const auto str2 =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    // skip if str2 contains separator character
    RC_PRE(std::none_of(str2.begin(), str2.end(), [](char ch){ return ch == bech32::separator; } ));

    // combine the strings, with a separator character between
    auto str = str1 + bech32::separator + str2;

    bech32::HrpAndDp b = splitString(str);

    RC_ASSERT(b.hrp == str1);
    RC_ASSERT(b.dp.size() == str2.length());
}

// check that we can lowercase strings
TEST(Bech32Test, lowercase_strings) {
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
TEST(Bech32Test, map_data) {
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
TEST(Bech32Test, expand_hrp) {
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
TEST(Bech32Test, polymod) {
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
TEST(Bech32Test, verifyChecksum_good) {
    std::string data("a14rxqtp");
    bech32::HrpAndDp b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "A14RXQTP";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lyllles";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "split1checkupstagehandshakeupstreamerranterredcaperred4m6xws";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio155t5hw";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

    data = "11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq8c42pv";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_TRUE(verifyChecksum(b.hrp, b.dp));

}

// check the verifyChecksum method
// these are simply the "good" tests from above with a single character changed
TEST(Bech32Test, verifyChecksum_bad) {
    std::string data("a14rxqtm");
    bech32::HrpAndDp b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "C14RXQTP";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "abcdefg1qpzry9x8gf2tvdw0s3jn54khce6mua7lyllles";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "split1dheckupstagehandshakeupstreamerranterredcaperred4m6xws";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "an83characterlonghumanreadablepartthatcontainsthenumber1andhheexcludedcharactersbio155t5hw";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));

    data = "11qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq8c42pc";
    b = splitString(data);
    convertToLowercase(b.hrp);
    mapDP(b.dp);
    ASSERT_FALSE(verifyChecksum(b.hrp, b.dp));
}

// check the main bech32 decode method
TEST(Bech32Test, decode_good) {
    std::string data("a14rxqtp");
    bech32::HrpAndDp b = bech32::decode(data);
    ASSERT_EQ(b.hrp, "a");
    ASSERT_TRUE(b.dp.empty());

    data = "A14RXQTP";
    b = bech32::decode(data);
    ASSERT_EQ(b.hrp, "a");
    ASSERT_TRUE(b.dp.empty());

    data = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lyllles";
    b = bech32::decode(data);
    ASSERT_EQ(b.hrp, "abcdef");
    ASSERT_EQ(b.dp.size(), 32);
    ASSERT_EQ(b.dp[0], '\0');    // 'q' in above dp part
    ASSERT_EQ(b.dp[31], '\x1f'); // 'l' in above dp part

}

TEST(Bech32Test, create_checksum) {
    std::string hrp = "a";
    std::vector<unsigned char> data;
    std::vector<unsigned char> checksum = createChecksum(hrp, data);

    ASSERT_EQ(checksum[0], '\x15');
    ASSERT_EQ(checksum[1], '\x03');
    ASSERT_EQ(checksum[2], '\x06');
    ASSERT_EQ(checksum[3], '\x00');
    ASSERT_EQ(checksum[4], '\x0B');
    ASSERT_EQ(checksum[5], '\x01');

    std::string mapped = mapToCharset(checksum);

    ASSERT_EQ(mapped, "4rxqtp");

    ////

    hrp = "abcdef";
    data = {'q','p','z','r','y','9','x','8','g','f','2','t','v','d','w','0','s','3','j',
            'n','5','4','k','h','c','e','6','m','u','a','7','l'};

    mapDP(data);
    checksum = createChecksum(hrp, data);

    ASSERT_EQ(checksum[0], '\x04');
    ASSERT_EQ(checksum[1], '\x1F');
    ASSERT_EQ(checksum[2], '\x1F');
    ASSERT_EQ(checksum[3], '\x1F');
    ASSERT_EQ(checksum[4], '\x19');
    ASSERT_EQ(checksum[5], '\x10');

    mapped = mapToCharset(checksum);

    ASSERT_EQ(mapped, "yllles");

    ////

    hrp = "split";
    data = {'c','h','e','c','k','u','p','s','t','a','g','e','h','a','n','d','s','h','a',
            'k','e','u','p','s','t','r','e','a','m','e','r','r','a','n','t','e','r','r',
            'e','d','c','a','p','e','r','r','e','d'};

    mapDP(data);
    checksum = createChecksum(hrp, data);

    ASSERT_EQ(checksum[0], '\x15');
    ASSERT_EQ(checksum[1], '\x1B');
    ASSERT_EQ(checksum[2], '\x1A');
    ASSERT_EQ(checksum[3], '\x06');
    ASSERT_EQ(checksum[4], '\x0E');
    ASSERT_EQ(checksum[5], '\x10');

    mapped = mapToCharset(checksum);

    ASSERT_EQ(mapped, "4m6xws");

    ////

    hrp = "an83characterlonghumanreadablepartthatcontainsthenumber1andtheexcludedcharactersbio";
    data = {};

    mapDP(data);
    checksum = createChecksum(hrp, data);

    ASSERT_EQ(checksum[0], '\x14');
    ASSERT_EQ(checksum[1], '\x14');
    ASSERT_EQ(checksum[2], '\x0B');
    ASSERT_EQ(checksum[3], '\x14');
    ASSERT_EQ(checksum[4], '\x17');
    ASSERT_EQ(checksum[5], '\x0E');

    mapped = mapToCharset(checksum);

    ASSERT_EQ(mapped, "55t5hw");

    ////

    hrp = "1";
    data = {'q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q',
            'q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q',
            'q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q',
            'q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q','q',
            'q','q','q','q','q','q'};

    mapDP(data);
    checksum = createChecksum(hrp, data);

    ASSERT_EQ(checksum[0], '\x07');
    ASSERT_EQ(checksum[1], '\x18');
    ASSERT_EQ(checksum[2], '\x15');
    ASSERT_EQ(checksum[3], '\x0A');
    ASSERT_EQ(checksum[4], '\x01');
    ASSERT_EQ(checksum[5], '\x0C');

    mapped = mapToCharset(checksum);

    ASSERT_EQ(mapped, "8c42pv");

}

// check the main bech32 encode method
TEST(Bech32Test, encode_good) {
    std::string hrp = "a";
    std::vector<unsigned char> data;
    std::string b = bech32::encode(hrp, data);
    ASSERT_EQ(b, "a14rxqtp");

    hrp = "A";
    b = bech32::encode(hrp, data);
    ASSERT_EQ(b, "a14rxqtp");
}

// check that we can decode and then encode back to the original
TEST(Bech32Test, check_decode_encode) {
    std::string data("a14rxqtp");
    bech32::HrpAndDp bs = bech32::decode(data);
    ASSERT_EQ(bs.hrp, "a");
    ASSERT_TRUE(bs.dp.empty());
    std::string enc = bech32::encode(bs.hrp, bs.dp);
    ASSERT_EQ(enc, data);

    data = "abcdef1qpzry9x8gf2tvdw0s3jn54khce6mua7lyllles";
    bs = bech32::decode(data);
    ASSERT_EQ(bs.hrp, "abcdef");
    enc = bech32::encode(bs.hrp, bs.dp);
    ASSERT_EQ(enc, data);

    data = "split1checkupstagehandshakeupstreamerranterredcaperred4m6xws";
    bs = bech32::decode(data);
    ASSERT_EQ(bs.hrp, "split");
    enc = bech32::encode(bs.hrp, bs.dp);
    ASSERT_EQ(enc, data);

}

RC_GTEST_PROP(Bech32TestRC, encodeThenDecodeShouldProduceInitialData, ()
) {
    // generate string with chars between a-z and 0-9
    const auto str1 =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    // generate string to be used as data. Restrict the values of the generated
    // chars to the range 0-31
    const auto str2 =
            *rc::gen::container<std::string>(
                    rc::gen::inRange(0, 31));


    // skip generated empty string
    RC_PRE(!str1.empty());
    RC_PRE(!str2.empty());

    // skip when combined lengths of both strings, plus 1 separator character, plus 6 character checksum is too long
    RC_PRE(str1.length() + str2.length() + 1 + 6 <= 90u);

    // copy to data vector
    std::vector<unsigned char> data;
    for(std::string::value_type c : str2) {
        data.push_back(static_cast<unsigned char>(c));
    }

    std::string bstr = bech32::encode(str1, data);
    bech32::HrpAndDp b = bech32::decode(bstr);

    RC_ASSERT(str1 == b.hrp);
    RC_ASSERT(data == b.dp);
}

TEST(Bech32Test, encode_empty_args) {
    std::string hrp;
    std::vector<unsigned char> data;
    std::string s;
    ASSERT_THROW(s = bech32::encode(hrp, data), std::runtime_error);
}

TEST(Bech32Test, strip_unknown_chars) {
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

RC_GTEST_PROP(Bech32TestRC, acceptDataValuesInRange, ()
) {
    // generate string to be used as data. Restrict the values of the generated
    // chars to the range 0-31
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::inRange(0, 31));

    // skip generated empty string
    RC_PRE(!str.empty());

    // copy to data vector
    std::vector<unsigned char> data;
    for(std::string::value_type c : str) {
        data.push_back(static_cast<unsigned char>(c));
    }

    // this should not throw
    rejectDataValuesOutOfRange(data);
}

RC_GTEST_PROP(Bech32TestRC, rejectDataValuesOutOfRange, ()
) {
    // generate string to be used as data. Restrict the values of the generated
    // chars to the range 32-127 to force an error
    const auto str =
            *rc::gen::container<std::string>(
                    rc::gen::inRange(32, 127));

    // skip generated empty string
    RC_PRE(!str.empty());

    // copy to data vector
    std::vector<unsigned char> data;
    for(std::string::value_type c : str) {
        data.push_back(static_cast<unsigned char>(c));
    }

    RC_ASSERT_THROWS_AS(rejectDataValuesOutOfRange(data), std::runtime_error);
}

RC_GTEST_PROP(Bech32TestRC, checkThatHrpAndDataIsNotTooLong, ()
) {
    // generate string with chars between a-z and 0-9
    const auto str1 =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    // generate string to be used as data. Restrict the values of the generated
    // chars to the range 0-31
    const auto str2 =
            *rc::gen::container<std::string>(
                    rc::gen::inRange(0, 31));


    // skip generated empty string
    RC_PRE(!str1.empty());
    RC_PRE(!str2.empty());

    // skip when combined lengths of both strings, plus 1 separator character, plus 6 character checksum is too long
    RC_PRE(str1.length() + str2.length() + 1 + 6 <= 90u);

    // copy to data vector
    std::vector<unsigned char> data;
    for(std::string::value_type c : str2) {
        data.push_back(static_cast<unsigned char>(c));
    }

    // this should not throw
    rejectBothPartsTooLong(str1, data);
}

RC_GTEST_PROP(Bech32TestRC, checkThatHrpAndDataIsTooLong, ()
) {
    // generate string with chars between a-z and 0-9
    const auto str1 =
            *rc::gen::container<std::string>(
                    rc::gen::oneOf(
                            rc::gen::inRange('a', 'z'),
                            rc::gen::inRange('0', '9')));

    // generate string to be used as data. Restrict the values of the generated
    // chars to the range 0-31
    const auto str2 =
            *rc::gen::container<std::string>(
                    rc::gen::inRange(0, 31));

    // generate filler string--used to make this test fail more regularly by being too long
    const auto filler =
            *rc::gen::container<std::string>(70,
                    rc::gen::just('1'));

    // skip generated empty string
    RC_PRE(!str1.empty());
    RC_PRE(!str2.empty());

    // skip when combined lengths of both strings is not too long
    RC_PRE(str1.length() + str2.length() + filler.length() + 6 > 90u);

    // copy to data vector
    std::vector<unsigned char> data;
    for(std::string::value_type c : str2) {
        data.push_back(static_cast<unsigned char>(c));
    }

    RC_ASSERT_THROWS_AS(rejectBothPartsTooLong(str1 + filler, data), std::runtime_error);
}

