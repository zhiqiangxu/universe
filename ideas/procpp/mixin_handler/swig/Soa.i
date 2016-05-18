class SoaRequest
{
public:
    int client;
    GUID uuid;
    string json;
};

class SoaResponse
{
public:
    GUID uuid;
    string json;

    SoaResponse(const GUID& uuid) : uuid(uuid) { }
};


class SoaClientServer
{
public:

    bool listen(uint16_t port, int domain = AF_INET);
    void start();
};

%feature("director") SoaCallback;
%inline %{
    class SoaCallback
    {
    public:
        virtual ~SoaCallback() {}
        virtual void run(SoaRequest& request, SoaResponse& response) {}
    };

    class SoaServer : public SoaClientServer
    {
    protected:
        SoaCallback* _callback;

    public:
        SoaServer() : _callback(nullptr)
        {
            EventManager::on<Soa::ON_REQUEST>(Utils::to_function([this](SoaRequest& request, SoaResponse& response) {
                if (_callback) _callback->run(request, response);
            }));

        }

        void on(char* event, SoaCallback* cb)
        {
            if (strcasecmp(event, "request") == 0) {
                if (_callback) delete _callback;
                _callback = cb;
            } else {
                exit(1);
            }
        }


    };

%}

/* client part */
#ifdef SWIGPHP

%typemap(in) vector<string>&    (vector<string> vec)
{
    zval** data;
    HashTable* hash;
    HashPosition ptr;

    hash = Z_ARRVAL_PP($input);

    for (
        zend_hash_internal_pointer_reset_ex(hash, &ptr);
        zend_hash_get_current_data_ex(hash, (void**)&data, &ptr) == SUCCESS;
        zend_hash_move_forward_ex(hash, &ptr)
    )
    {
        zval temp, *str;

        bool is_str = true;

        if (Z_TYPE_PP(data) != IS_STRING)
        {
            temp = **data;
            zval_copy_ctor(&temp);
            convert_to_string(&temp);
            str = &temp;
            is_str = false;
        }
        else str = *data;

        vec.push_back(Z_STRVAL_P(str));

        if (!is_str) zval_dtor(&temp);
    }

    $1 = &vec;
}

#endif


namespace C {
    class Soa
    {
    public:
        Soa(const string& address, uint16_t port, bool auto_reconnect);
    };

}

%feature("director") SoaClientCallback;
%inline %{
    class SoaClientCallback
    {
    public:
        virtual ~SoaClientCallback() {}
        virtual void run(string json) {}
    };


    class SoaClient : public C::Soa
    {
    public:
        SoaClient(const string& address, uint16_t port, bool auto_reconnect = true) : C::Soa(address, port, auto_reconnect) {}

        string call(const string& json_request, SoaClientCallback* cb)
        {
            GUID request_id;

            this->cmd<P::Client::Soa, P::Client::Soa::packet_type::JSON>(request_id, Utils::to_function([cb](string& json_response){
                cb->run(json_response);
            }), json_request);

            return request_id.to_string();
        }

        int wait(const vector<string>& requests, int milliseconds/*TODO this will cause swig typemap problem = 50*/)
        {
            vector<GUID> request_guids;

            GUID guid;
            for (auto& uuid : requests) {
                if (GUID::from_string(uuid, guid)) request_guids.push_back(guid);
            }

            return Client::wait(request_guids, milliseconds);
        }
    };
%}

#ifdef SWIGPHP

%clear vector<string>;

#endif


