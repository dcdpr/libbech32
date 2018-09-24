#include <gtest/gtest.h>

#include "vout.h"

TEST(VoutTest, construct_valid_vout) {

    ASSERT_NO_THROW(std::unique_ptr<Vout>(new Vout(0)));
    ASSERT_NO_THROW(std::unique_ptr<Vout>(new Vout(1)));
    ASSERT_NO_THROW(std::unique_ptr<Vout>(new Vout(10)));

}

TEST(VoutTest, vout_with_negative_index_is_invalid) {

    ASSERT_THROW(std::unique_ptr<Vout>(new Vout(-1)), std::runtime_error);

}

TEST(VoutTest, test_vout_equality) {

    Vout v1(3);
    Vout v2(3);
    Vout v3(4);

    ASSERT_EQ(v1, v2);
    ASSERT_NE(v1, v3);
}

