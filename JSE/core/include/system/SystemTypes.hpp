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

    enum { INT_TYPE, FLOAT_TYPE };

    struct AnyValue
    {
        AnyValue(const float a0) :type(FLOAT_TYPE), floatVal(a0) {}
        AnyValue(const int a0) :type(INT_TYPE), floatVal(a0) {}
        AnyValue() :type(INT_TYPE), intVal(0) {}

        int type;
        union {
            int intVal;
            float floatVal;
        };

        AnyValue& operator=(const int a0)
        {
            type = INT_TYPE;
            intVal = a0;
            return *this;
        }

        AnyValue& operator=(const float a0)
        {
            type = FLOAT_TYPE;
            floatVal = a0;

            return *this;
        }

        bool operator<(const AnyValue& other)
        {
            switch (type)
            {
            case INT_TYPE: return intVal < other.intVal;
            case FLOAT_TYPE: return floatVal < other.floatVal;
            default: return true;
            }
        }
    };
}

#endif