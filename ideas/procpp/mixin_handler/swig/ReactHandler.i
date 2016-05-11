%module(directors="1") ReactHandler

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
******* Trie *********
*********************/
%include "Trie.i"


