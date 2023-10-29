#include "Downloader.h"
#include <curl/curl.h>
#include <fstream>

static size_t writeData(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

bool downloadFile(const std::string& url, const std::string& outputPath) {
    CURL* curl;
    FILE* fp;
    CURLcode res;

    curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    errno_t err = fopen_s(&fp, outputPath.c_str(), "wb");
    if (err != 0) {
        // Handle error
        curl_easy_cleanup(curl);
        return false;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);
    fclose(fp);

    return (res == CURLE_OK);
}
