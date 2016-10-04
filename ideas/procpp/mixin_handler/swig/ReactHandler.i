%module(directors="1") ReactHandler

%{
#include "ReactHandler.h"
%}

%include "stdint.i"
%include "std_string.i"
%include "std_map.i"
%include "std_vector.i"
%include "std_pair.i"
%include "typemaps.i"

using namespace std;
namespace std
{
    %template(StringMap) map<string, string>;
    %template(StringPair) pair<string, string>;
    %template(StringPairVector) vector<pair<string, string>>;
}

/*****************************
******* ClientServer *********
*****************************/
%include "ClientServer.i"


/*********************
***** WebSocket ******
*********************/
%include "Websocket.i"

/*********************
******* Http *********
*********************/
%include "Http.i"


/*********************
******* Socks ********
*********************/
%include "Socks.i"

/*********************
******* Soa **********
*********************/
%include "Soa.i"

/*********************
******* Proxy ********
*********************/
%include "Proxy.i"

/*********************
******* Remote ********
*********************/
%include "Remote.i"


/*********************
******* Trie *********
*********************/
%include "Trie.i"


