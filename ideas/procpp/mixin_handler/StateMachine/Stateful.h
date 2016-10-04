#pragma once
#include <map>
using namespace std;

template <typename State>
class IStateful
{
public:

    virtual void set_state(int id, State state) = 0;
    virtual bool has_state(int id) = 0;
    virtual State get_state(int id, bool* exists) = 0;
    virtual size_t count_state() = 0;

    virtual void erase_state(int id) = 0;
    virtual void clear_state() = 0;
};

template <typename State>
class Stateful : public IStateful<State>
{
public:


    virtual void set_state(int id, State state) override;
    virtual bool has_state(int id) override;
    virtual State get_state(int id, bool* exists = nullptr) override;
    virtual size_t count_state() override;

    virtual void erase_state(int id) override;
    virtual void clear_state() override;

private:
    map<int, State> _state;
};

#include "StateMachine/Stateful.hpp"
