#include <stdexcept>
#include "transactionPosition.h"

TransactionPosition::TransactionPosition(int index) {
    if(index < 0)
        throw std::runtime_error("index can not be less than zero");

    this->index = index;
}

int TransactionPosition::value() {
    return index;
}

bool TransactionPosition::operator==(const TransactionPosition &rhs) const {
    return index == rhs.index;
}

bool TransactionPosition::operator!=(const TransactionPosition &rhs) const {
    return !(rhs == *this);
}
