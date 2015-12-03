#include <map>
#include <vector>
#include <stack>
#include <set>
#include <array>
#include <iostream>
#include <algorithm>

using namespace std;

void test_map()
{
	map<string, int> grades;
	grades["xiao_a"] = 1;
	grades["xiao_b"] = 2;

	cout << "xiao_a: " << grades["xiao_a"] << endl;
	cout << "xiao_b: " << grades["xiao_b"] << endl;
}

void test_vector()
{
	vector<int> vec;

	vec.push_back(1);
	vec.push_back(2);

	vector<int>::iterator v = vec.begin();
	while(v != vec.end()) {
		cout << *v << endl;
		v++;
	}
}

void test_stack()
{
	stack<int> s;
	s.push(1);
	s.push(2);
	while(!s.empty())
	{
		cout << s.top() << endl;
		s.pop();
	}
}

void test_set()
{
	set<int> s;
	s.insert(1);
	s.insert(1);
	s.insert(1);

	set<int>::iterator v = s.begin();
	while(v != s.end()) {
		cout << *v << endl;
		v++;
	}
	s.clear();

}

void test_array()
{
	array<int, 5> ia { 1,5,4,3,2 };
	sort(ia.begin(), ia.end());

	for (const auto &element : ia) cout << element << endl;
}

int main()
{
	//test_map();
	//test_vector();
	//test_stack();
	//test_set();
	test_array();
	return 0;
}
