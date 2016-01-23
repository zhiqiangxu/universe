#pragma once
#include <string>
#include <map>
using namespace std;

class IBufferable
{
public:
	/******buffer*******/
	virtual bool need_buf(int id, string& message, bool cond) = 0;
	virtual bool append_buf(int id, string& message) = 0;
	virtual size_t count_buf() = 0;
	virtual bool has_buf(int id) = 0;
	virtual string get_buf(int id, bool clear) = 0;

	virtual void erase_buf(int id) = 0;
	virtual void clear_buf() = 0;

};


class Bufferable : public IBufferable
{
public:
	virtual bool need_buf(int id, string& message, bool cond) override;
	virtual bool append_buf(int id, string& message) override;
	virtual size_t count_buf() override;
	virtual bool has_buf(int id) override;
	virtual string get_buf(int id, bool clear = true) override;

	virtual void erase_buf(int id) override;
	virtual void clear_buf() override;

private:
	map<int, string> _buf;
};
