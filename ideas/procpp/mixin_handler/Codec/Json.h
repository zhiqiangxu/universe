#pragma once

#include <map>
#include <vector>
#include <string>
#include "Utils.h"
using namespace std;


class Json
{
public:
    enum class ValueType
    {
        Object,
        Array,
        Number,
        String,
        Bool,
        Null
    };

    struct Value
    {
        ValueType type;
        int64_t number;
        string str;
        vector<Value> arr;
        map<string, Value> obj;
    };

    static Encoding detect_encoding(const string& json_string);
    static Value decode(const string& json_string);
    static string encode(const Value& json_value);
};

