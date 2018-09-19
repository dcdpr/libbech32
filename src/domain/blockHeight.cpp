#include <stdexcept>
#include "blockHeight.h"

BlockHeight::BlockHeight(int inHeight) {
    if(inHeight < 0)
        throw std::runtime_error("index can not be less than zero");

    height = inHeight;
}

int BlockHeight::value() {
    return height;
}

bool BlockHeight::operator==(const BlockHeight &rhs) const {
    return height == rhs.height;
}

bool BlockHeight::operator!=(const BlockHeight &rhs) const {
    return !(rhs == *this);
}
