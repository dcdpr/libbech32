#include <stdexcept>
#include "vout.h"

Vout::Vout(int inIndex) {
    if(inIndex < 0)
        throw std::runtime_error("index can not be less than zero");

    index = inIndex;
}

int Vout::value() {
    return index;
}

bool Vout::operator==(const Vout &rhs) const {
    return index == rhs.index;
}

bool Vout::operator!=(const Vout &rhs) const {
    return !(rhs == *this);
}
