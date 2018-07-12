#include "encodeOpReturnData.h"

namespace {

    const int MAX_OP_RETURN_LENGTH = 80;

}

std::string encodeOpReturnData(const std::string &data) {
    //TODO sanitize input?
    if(data.length() > MAX_OP_RETURN_LENGTH) {
        return "";
    }
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (char c : data)
    {
        ss << std::setw(2) << static_cast<unsigned>(c);
    }
    return ss.str();
}
