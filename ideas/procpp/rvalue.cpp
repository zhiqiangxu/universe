#include <iostream>

auto function(int&& i)
{
	std::cout << i << std::endl;
}

int main()
{
	function(10);
	return 0;
}
