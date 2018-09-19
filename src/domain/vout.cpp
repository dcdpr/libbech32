#include <stdexcept>
#include "vout.h"

Vout::Vout(int index) {
    if(index < 0)
        throw std::runtime_error("index can not be less than zero");

    this->index = index;
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
