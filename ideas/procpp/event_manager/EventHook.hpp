#pragma once
#include <map>
#include <functional>
#include <map>
#include <vector>
using namespace std;


/**************************************************************************/
/*******************EventHook START***********************************/
/**************************************************************************/
template <typename group, typename event_name, typename ... Args>
class EventHook
{
	using CB_TYPE = function<void(Args...)>;
	using SELF_TYPE = EventHook<group, event_name, Args...>;

	uint64_t _id;
	map<uint64_t, CB_TYPE> _callbacks;
	vector<uint64_t> _cancel_id;
	bool _firing = false;

	//declare
	static SELF_TYPE* S_instance;

	EventHook() {}

public:

	static SELF_TYPE& get_instance()
	{
		if (S_instance == nullptr) S_instance = new EventHook;

		return *S_instance;
	}

	template<typename CB>
	uint64_t  attach(CB&& cb)
	{
		auto id = _id++;
		_callbacks[id] = cb;

		return id;
	}

	void fire(Args... args)
	{
		_firing = true;
		for (auto& it : _callbacks)
			it.second(args...);

		_firing = false;

		for (auto id : _cancel_id) _callbacks.erase(id);

		_callbacks.clear();
	}

	void detach(uint64_t id)
	{
		if (_firing) _cancel_id.push_back(id);
		else _callbacks.erase(id);
	}
};

//define
template <typename group, typename ev, typename ... Args>
EventHook<group, ev, Args...>* EventHook<group, ev, Args...>::S_instance;

/**************************************************************************/
/*******************EventHook END******************************************/
/**************************************************************************/

/**************************************************************************/
/*******************EventHookGlobal START**********************************/
/**************************************************************************/

template <typename event_name, typename ... Args>
class EventHookGlobal
{
	using CB_TYPE = function<void(Args...)>;
	using SELF_TYPE = EventHookGlobal<event_name, Args...>;

	uint64_t _id;
	map<uint64_t, CB_TYPE> _callbacks;

	//declare
	static SELF_TYPE* S_instance;

	EventHookGlobal() {}

public:

	static SELF_TYPE& get_instance()
	{
		if (S_instance == nullptr) S_instance = new EventHookGlobal;

		return *S_instance;
	}

	template<typename CB>
	uint64_t  attach(CB&& cb)
	{
		auto id = _id++;
		_callbacks[id] = cb;

		return id;
	}

	void fire(Args... args)
	{
		for (auto& it : _callbacks)
			it.second(args...);
	}

	void detach(uint64_t id)
	{
		_callbacks.erase(id);
	}
};

//define
template <typename event_name, typename ... Args>
EventHookGlobal<event_name, Args...>* EventHookGlobal<event_name, Args...>::S_instance;

/**************************************************************************/
/*******************EventHookGlobal END************************************/
/**************************************************************************/


