#pragma once
#include "Protocol.h"
#include <uuid/uuid.h>
#include <map>
using namespace std;

/*
 * uuid_t
 * size_t
 * json
**/

class SoaRequest
{
public:
    int client;
    GUID uuid;
    string json;

    static string to_string(const GUID& uuid, const string& json);
};

class SoaResponse
{
public:
    GUID uuid;
    string json;

    SoaResponse(bool generate = false) { if (generate) uuid.generate(); }
    SoaResponse(const GUID& uuid) : uuid(uuid) { }

    virtual string to_string();
    // required by delete
    virtual ~SoaResponse() {};
};



class ISoa
{
public:
    virtual SoaRequest parse_request(int client, StreamReader& s) = 0;

};

class Soa : public Protocol, public ISoa
{
public:
    using Protocol::Protocol;

    class ON_REQUEST {};

    virtual void on_connect(int client) override;
    virtual void on_message(int client, string message) override;
    virtual void on_close(int client) override;
    virtual SoaRequest parse_request(int client, StreamReader& s) override;

};
