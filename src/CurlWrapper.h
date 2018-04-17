#ifndef TXREF_CURLWRAPPER_H
#define TXREF_CURLWRAPPER_H

#include <string>

class CurlWrapper {
public:
    CurlWrapper();
    ~CurlWrapper();

    /**
     * Downloads contents from URL with HTTP GET and store in in a std::string
     * @param url The URL to download
     * @return The download result
     */
    std::string download(const std::string& url);

private:
    void* curl;
};


#endif //TXREF_CURLWRAPPER_H
