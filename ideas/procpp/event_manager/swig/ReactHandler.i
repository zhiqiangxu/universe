%module ReactHandler

%{
#include "ReactHandler.h"
%}

%include "stdint.i"
%include "std_string.i"
%include "std_map.i"

using namespace std;
namespace std
{
    %template(StringMap) map<string, string>;
}

class HttpClientServer
{
public:
    bool listen(uint16_t port, int domain = AF_INET);
    void start();
};

class HttpRequest
{
public:
	int client;
	string method;
	string uri;
	string http_version;
	map<string, string> headers;
	string body;
};

class HttpResponse
{
public:
	int status_code = 200;
	string reason_phrase;
	map<string, string> headers;
	string body;
};


#ifdef SWIGPYTHON

%extend HttpClientServer {

    void on(char* event, PyObject* callback)
    {
        if (strcasecmp(event, "request") == 0) {
            Py_XINCREF(callback);
            self->on<Http::ON_REQUEST>(Utils::to_function([callback](HttpRequest& req, HttpResponse& resp) {

                auto swig_req = SWIG_NewPointerObj(SWIG_as_voidptr(&req), SWIGTYPE_p_HttpRequest, 0);
                auto swig_resp = SWIG_NewPointerObj(SWIG_as_voidptr(&resp), SWIGTYPE_p_HttpResponse, 0);
                auto arg_list = Py_BuildValue("(OO)", swig_req, swig_resp);

                PyEval_CallObject(callback, arg_list);

                Py_DECREF(arg_list);
                Py_DECREF(swig_req);
                Py_DECREF(swig_resp);
            }));
        } else {
            exit(1);
        }
    }

}

#endif

#ifdef SWIGPHP

%extend HttpClientServer {

    void on(char* event, int callback)
    {
        if (strcasecmp(event, "request") == 0) {
            std::cout << "request" << std::endl;
            //Py_XINCREF(callback);
            self->on<Http::ON_REQUEST>(Utils::to_function([callback](HttpRequest& req, HttpResponse& resp) {

/*
                auto swig_req = SWIG_NewPointerObj(SWIG_as_voidptr(&req), SWIGTYPE_p_HttpRequest, 0);
                auto swig_resp = SWIG_NewPointerObj(SWIG_as_voidptr(&resp), SWIGTYPE_p_HttpResponse, 0);
                auto arg_list = Py_BuildValue("(OO)", swig_req, swig_resp);

                PyEval_CallObject(callback, arg_list);

                Py_DECREF(arg_list);
                Py_DECREF(swig_req);
                Py_DECREF(swig_resp);
*/
            }));

        } else {
            exit(1);
        }
    }

}

#endif

