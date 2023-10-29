#include "Base64Helper.h"
#include <iostream>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

std::string Base64Decode(const std::string& input) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // No newlines
    BIO* bio = BIO_new_mem_buf(input.data(), static_cast<int>(input.size()));
    bio = BIO_push(b64, bio);

    std::string output;
    char buffer[128];
    int decodedLength = 0;
    while ((decodedLength = BIO_read(bio, buffer, 128)) > 0) {
        output.append(buffer, decodedLength);
    }

    BIO_free_all(bio);
    return output;
}

