#ifndef BASE64_H
#define BASE64_H

#include <string>
#include <vector>

// whats base64 encoding - https://www.youtube.com/watch?v=8qkxeZmKmOY

namespace Base64 {
    std::string base64Encode(const std::string &s);

    const std::string &SALT1 = "LM::TB::BB";
    const std::string &SALT2 = "_:/_77";
    const std::string &SALT3 = "lint=wowC++";

    std::string encryptB64(std::string) {
        s = SALT1 + s + SALT2 + s + SALT3;
        s = base64Encode(s);
        s += SALT1;
        s.insert(6, SALT2);
        s = base64Encode(s);
        s += SALT3 + SALT2;
        s = base64Encode(s);
        s.insert(2, "R");
        s.insert(5, "H");
        return s;
    }

    const std::string BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    std::string base64Encode(const std::string &s) {
        // to be done
    }
}

#endif // BASE64_H
