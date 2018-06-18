#include "CurlWrapper.h"

#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/curlbuild.h>
#include <sstream>
#include <iostream>

namespace {

    size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
        std::string data(static_cast<const char*>(ptr), size * nmemb);
        *(static_cast<std::stringstream*>(stream)) << data << std::endl;
        return size * nmemb;
    }

}

CurlWrapper::CurlWrapper()
        : curl(curl_easy_init()) {
}

CurlWrapper::~CurlWrapper() {
    curl_easy_cleanup(curl);
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"

std::string CurlWrapper::download(const std::string &url) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    // follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    // prevent "longjmp causes uninitialized stack frame" bug
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");

    std::stringstream out;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

    // Perform the request, res will get the return code
    CURLcode res = curl_easy_perform(curl);

    // Check for errors
    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    }
    return out.str();
}

#pragma clang diagnostic pop
