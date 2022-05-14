#ifndef JSE_SYSTEM_TYPES
#define JSE_SYSTEM_TYPES

#include <vector>
#include <string>
#include <cstddef>

namespace jse {

    typedef std::string String;
    typedef std::wstring WString;
    typedef std::vector<uint8_t> ByteVector;

    typedef uint32_t Flag;

    typedef std::vector<String> StringList;
    typedef StringList::iterator StringListIt;

    typedef std::vector<WString> WStringList;
    typedef WStringList::iterator WStringListIt;

    typedef std::vector<int> IntList;
    typedef IntList::iterator IntListIt;

    typedef int32_t i32;
    typedef uint32_t u32;

    typedef int64_t i64;
    typedef uint64_t u64;

    typedef int8_t i8;
    typedef uint8_t u8;


}

#endif