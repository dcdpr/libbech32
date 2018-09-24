#include <gtest/gtest.h>

#include "blockHeight.h"

TEST(BlockHeightTest, construct_valid_blockheight) {

    ASSERT_NO_THROW(std::unique_ptr<BlockHeight>(new BlockHeight(0)));
    ASSERT_NO_THROW(std::unique_ptr<BlockHeight>(new BlockHeight(1)));
    ASSERT_NO_THROW(std::unique_ptr<BlockHeight>(new BlockHeight(10)));

}

TEST(BlockHeightTest, blockheight_with_negative_index_is_invalid) {

    ASSERT_THROW(std::unique_ptr<BlockHeight>(new BlockHeight(-1)), std::runtime_error);

}

TEST(BlockHeightTest, test_blockheight_equality) {

    BlockHeight bh1(3);
    BlockHeight bh2(3);
    BlockHeight bh3(4);

    ASSERT_EQ(bh1, bh2);
    ASSERT_NE(bh1, bh3);
}
