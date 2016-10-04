#pragma once
#include <string>
#include "Utils.h"
using namespace std;

//https://en.wikipedia.org/wiki/UTF-16

/*******调用流程******
 ******1. 如果端序已知，直接调用length/decode_char_length/decode_char等****
 ******2. 如果端序未知，先调用detect_endian，然后goto 1************/
class Utf16
{
public:
    //返回字符数
    template <Endian endian>
    static size_t length(const string& s);
    //返回指定offset处字符的尺寸
    template <Endian endian>
    static size_t decode_char_length(const string& s, size_t& offset);
    //成功返回true，返回下一个字符ch，并更新offset
    template <Endian endian>
    static bool decode_char(const string& s, string& ch, size_t& offset);

    //是否相同端序
    static Endian detect_endian(const string& s);

    static bool has_surrogate_pair(uint16_t codepoint) { return !( codepoint <= 0xD7FF || (codepoint >= 0xE000 && codepoint <= 0xFFFF) ); }

    static bool to_codepoint(uint16_t pair1, uint16_t pair2, uint32_t& result);
};

#include "Codec/Utf16.hpp"
