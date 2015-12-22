#include <iostream>
#include <vector>
#include <memory>
using namespace std;

/***
** If you want to store pointers in containers use unique_ptr if the
container becomes owner of the pointed-to object, or shared_ptr if the container
shares ownership with other owners. Do not use the old deprecated auto_ptr
class in containers because it does not implement copying correctly (as far as the
STL is concerned).
***/
auto store_pointer_to_container()
{
	vector<unique_ptr<int>> vec;

	unique_ptr<int> ptr(new int(1));
	vec.push_back(move(ptr));//unique_ptr没有拷贝构造，必须move

	//move后ptr内容已被转移，不能再获取
	//cout << *ptr << endl;
	cout << *vec[0] << endl;
}

int main()
{
	store_pointer_to_container();
	return 0;
}
