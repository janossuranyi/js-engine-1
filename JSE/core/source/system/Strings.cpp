#include "system/Strings.hpp"
#include "system/SystemTypes.hpp"

namespace jse {

    StringList split(String s, String delimiter) {
        size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        String token;
        StringList res;

        while ((pos_end = s.find(delimiter, pos_start)) != String::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    }
}
