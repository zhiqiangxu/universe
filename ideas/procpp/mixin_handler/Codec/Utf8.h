#pragma once
#include <string>
using namespace std;

class Utf8
{
public:
    //返回字符数
    static size_t length(const string& s);
    //返回指定offset处字符的尺寸，并可返回Unicode码字
    static size_t decode_char_length(const string& s, size_t offset, uint32_t* p_codepoint = 0);
    //成功返回true，返回下一个字对应的bytes，并更新offset
    static bool decode_char(const string& s, string& ch, size_t& offset);
    static string encode(uint32_t unicode);
};
