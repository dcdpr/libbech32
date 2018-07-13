#ifndef TXREF_ENCODEOPRETURNDATA_H
#define TXREF_ENCODEOPRETURNDATA_H

#include <string>
#include <sstream>
#include <iomanip>

// The data returned by OP_RETURN needs to be UTF-8, hex-encoded. Assuming UTF-8 for now.
std::string encodeOpReturnData(const std::string &data);

#endif //TXREF_ENCODEOPRETURNDATA_H
