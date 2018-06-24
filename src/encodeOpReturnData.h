#ifndef TXREF_ENCODEOPRETURNDATA_H
#define TXREF_ENCODEOPRETURNDATA_H

#include <string>
#include <sstream>
#include <iomanip>

// The data returned by OP_RETURN needs to be UTF-8, hex-encoded. Assuming UTF-8 for now.
inline std::string encodeOpReturnData(const std::string &data) {
    //TODO sanitize input? what data is acceptable--just url-valid data?
    if(data.length() > 80) {
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


#endif //TXREF_ENCODEOPRETURNDATA_H
