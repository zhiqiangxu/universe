#include <map>
#include <vector>
#include <stack>
#include <set>
#include <array>
#include <queue>
#include <iostream>
#include <list>
#include <bitset>
#include <iterator>
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

void test_queue()
{
	queue<string> q;
	q.push("q1");
	q.push("q2");
	while (!q.empty())
	{
		auto e = q.front();
		cout << e << endl;
		q.pop();
	}
}

void test_priority_queue()
{
	//In the case of priority ties,
	//the order in which elements are removed is undefined.
	priority_queue<string> q;
	q.push("q1");
	q.push("q2");
	while (!q.empty())
	{
		auto e = q.top();
		cout << e << endl;
		q.pop();
	}
}

void test_list()
{
	// Store the a words in the main dictionary.
	list<string> dictionary{ "aardvark", "ambulance" };
	// Store the b words.
	list<string> bWords{ "bathos", "balderdash" };
	// Add the c words to the main dictionary
	dictionary.push_back("canticle");
	dictionary.push_back("consumerism");
	// splice the b words into the main dictionary.
	if (bWords.size() > 0) {
		// 4.9对c++14的支持是实验性质的，缺乏部分方法如cend
		// http://stackoverflow.com/questions/31038061/support-of-stdcbeginin-c14
		// Get an iterator to the last b word.
		auto iterLastB = --(end(bWords));
		// Iterate up to the spot where we want to insert bs.
		auto it = begin(dictionary);
		for (; it != end(dictionary); ++it) {
			if (*it > *iterLastB)
				break;
		}
		// Add in the bwords. This action removes the elements from bWords.
		dictionary.splice(it, bWords);
	}
	// print out the dictionary
	for (const auto& word : dictionary) {
		cout << word << endl;
	}
}

auto bitset_test()
{
	bitset<10> myBitset;
	myBitset.set(3);
	myBitset.set(6);
	myBitset[8] = true;
	myBitset[9] = myBitset[3];
	if (myBitset.test(3)) {
		cout << "Bit 3 is set!"<< endl;
	}
	cout << myBitset << endl;
}

int main()
{
	//test_map();
	//test_vector();
	//test_stack();
	//test_set();
	//test_array();
	//test_queue();
	//test_priority_queue();
	test_list();
	bitset_test();
	return 0;
}
