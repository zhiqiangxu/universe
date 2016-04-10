#pragma once
#include <string>
using namespace std;

class Utf8
{
public:
	//返回字符数
	static size_t length(const string& s);
	//返回指定offset处字符的尺寸
	static size_t decode_char_length(const string& s, size_t offset);
	//成功返回true，返回下一个字符ch，并更新offset
	static bool decode_char(const string& s, string& ch, size_t& offset);
};
