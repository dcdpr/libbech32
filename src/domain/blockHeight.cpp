#include <stdexcept>
#include "blockHeight.h"

BlockHeight::BlockHeight(int index) {
    if(index < 0)
        throw std::runtime_error("index can not be less than zero");

    this->index = index;
}

int BlockHeight::value() {
    return index;
}

bool BlockHeight::operator==(const BlockHeight &rhs) const {
    return index == rhs.index;
}

bool BlockHeight::operator!=(const BlockHeight &rhs) const {
    return !(rhs == *this);
}
