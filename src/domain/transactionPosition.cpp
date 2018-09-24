#include <stdexcept>
#include "transactionPosition.h"

TransactionPosition::TransactionPosition(int inPosition) {
    if(inPosition < 0)
        throw std::runtime_error("index can not be less than zero");

    position = inPosition;
}

int TransactionPosition::value() {
    return position;
}

bool TransactionPosition::operator==(const TransactionPosition &rhs) const {
    return position == rhs.position;
}

bool TransactionPosition::operator!=(const TransactionPosition &rhs) const {
    return !(rhs == *this);
}
