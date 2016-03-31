#pragma once

#include <string>
using namespace std;

class Base64
{
public:
	static string encode(const string& bindata);
	static string decode(const string& ascdata);
};
