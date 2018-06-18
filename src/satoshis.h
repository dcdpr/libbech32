#ifndef TXREF_SATOSHIS_H
#define TXREF_SATOSHIS_H

// BTC <-> satoshi conversions

#include <cstdint>

const int SATOSHIS_PER_BTC = 100000000;

inline int64_t btc2satoshi(double value) {
    return static_cast<int64_t>(value * SATOSHIS_PER_BTC + (value < 0.0 ? -.5 : .5));
}

inline double satoshi2btc(int64_t value) {
    return static_cast<double>(value) / SATOSHIS_PER_BTC;
}


#endif //TXREF_SATOSHIS_H
