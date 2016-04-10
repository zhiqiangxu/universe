#include "DataStructure/Trie.h"
#include "Codec/Utf8.h"

Trie::Trie(vector<string>&& keywords)
{
	for (auto& keyword : keywords) {
		if (keyword.length() == 0) continue;

		//it's a bug if use reference here:
		//
		//  http://stackoverflow.com/a/7181452
		auto node = &_root;
		size_t offset = 0;
		string ch;

		while (Utf8::decode_char(keyword, ch, offset)) {
			auto next = node->get_next(ch);
			if (!next) {
				next = &node->add_next(ch, Node());
			}

			node = next;
		}

		node->set_value(keyword);
	}
}

vector<string> Trie::search(const string& text)
{
	map<string, bool> result;
	size_t offset = 0;
	string ch;


	while (Utf8::decode_char(text, ch, offset)) {
		auto node = &_root;
		auto next_offset = offset;

		do {
			node = node->get_next(ch);
			if (node) {
				if (node->get_value()) {
					result[*(node->get_value())] = true;
				}

				if (!Utf8::decode_char(text, ch, next_offset)) break;

			} else break;
		} while (true);
	}

	vector<string> ret;
	for (auto& r : result) {
		ret.push_back(r.first);
	}
	return ret;
}

Trie::Node::Node(Trie::Node&& node)
{
	_next = move(node._next);
	_p_value = node._p_value;
	node._p_value = nullptr;
}

Trie::Node::~Node()
{
	if (_p_value) delete _p_value;
}

const string* Trie::Node::get_value()
{
	return _p_value;
}

void Trie::Node::set_value(const string& value)
{
	if (_p_value) delete _p_value;

	_p_value = new string(value);
}

Trie::Node* Trie::Node::get_next(const string& ch)
{
	auto it = _next.find(ch);
	if (it != _next.end()) {
		return &it->second;
	}

	return nullptr;
}

Trie::Node& Trie::Node::add_next(const string& ch, Trie::Node&& node)
{
	_next[ch] = move(node);

	return _next[ch];

}

void Trie::Node::operator=(Node&& node)
{
	_p_value = node._p_value;
	_next = move(node._next);
	node._p_value = nullptr;
}
