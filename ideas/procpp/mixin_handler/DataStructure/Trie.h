#pragma once
#include <vector>
#include <map>
#include <string>
using namespace std;


class Trie
{
public:

	
	Trie(vector<string>&& keywords);

	vector<string> search(const string& text);

protected:

	class Node
	{
	public:
		Node(const Node&) = delete;
		Node(Node&&);
		Node() : _p_value(nullptr) {}
		virtual ~Node();

		const string* get_value();
		void set_value(const string& value);
		Node* get_next(const string& ch);
		Node& add_next(const string& ch, Node&& node);
		void operator=(const Node& node) = delete;
		void operator=(Node&& node);

	protected:
		map<string, Node> _next;
		string* _p_value;
	};

	Node _root;
};
