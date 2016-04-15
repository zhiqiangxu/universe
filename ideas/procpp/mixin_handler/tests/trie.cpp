#include "Codec/Utf8.h"
#include "DataStructure/Trie.h"
#include <iostream>
#include "ReactHandler.h"
using namespace std;


int main()
{
	string s = "中国人";

	string result;
	size_t offset = 0;
	while (Utf8::decode_char(s, result, offset)) {
		cout << s << endl;
		cout << "n_chars = " << Utf8::length(s) << " length = " << s.length() << " " << "ok" << " offset = " << offset << endl;
	}

	vector<string> keywords({"中国", "国人", "ab", "bc", "cd"});
	for (int i = 0; i < 1000; i++ ) {
		keywords.push_back(Utils::rand_string(13));
	}

	string text;
	for (int i = 0; i < 1; i++) {
		text += "中国人abc isn cd";
	}

	auto start = Utils::now();
	Trie trie(move(keywords));

	auto search_result = trie.search(text);

	auto end = Utils::now();
	std::chrono::duration<double> elapsed = end - start;
	cout << "tooks " << elapsed.count() << "s\r\n";

	
	for (auto& keyword : search_result) {
		cout << keyword << endl;
	}
	return 0;
}
