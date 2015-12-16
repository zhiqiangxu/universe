#include <string>
#include <iostream>


std::string operator"" _demo(unsigned long long v)
{
	return "hello world literal";
}


int main()
{
	std::cout << 12_demo << std::endl;
	std::cout << 12'1'23 << std::endl;
	return 0;
}



