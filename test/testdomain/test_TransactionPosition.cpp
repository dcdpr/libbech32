#include <gtest/gtest.h>

#include "transactionPosition.h"

TEST(TransactionPositionTest, construct_valid_TransactionPosition) {

    ASSERT_NO_THROW(std::unique_ptr<TransactionPosition>(new TransactionPosition(0)));
    ASSERT_NO_THROW(std::unique_ptr<TransactionPosition>(new TransactionPosition(1)));
    ASSERT_NO_THROW(std::unique_ptr<TransactionPosition>(new TransactionPosition(10)));

}

TEST(TransactionPositionTest, TransactionPosition_with_negative_index_is_invalid) {

    ASSERT_THROW(std::unique_ptr<TransactionPosition>(new TransactionPosition(-1)), std::runtime_error);

}

TEST(TransactionPositionTest, test_transactionposition_equality) {

    TransactionPosition tp1(3);
    TransactionPosition tp2(3);
    TransactionPosition tp3(4);

    ASSERT_EQ(tp1, tp2);
    ASSERT_NE(tp1, tp3);
}
