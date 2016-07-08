# Reactor server framework for mixin programing


**react handler** offers two fundamental features:
 
- a **generic** server framework that follows react pattern(similar to the famous swoole framework), written in c++；
- you can use it in various scripting languages such as php/python and more；

It is **generic** in that it can be used for all kinds of servers, such as socks server, http server, etc.

All you need to do to support another kind of server is to offer a class that extends `Protocol`(tcp) or `UProtocol`(udp).


-------------------

## Demo

`http` server in php: 

``` <?php
require("ReactHandler.php");
class PhpCallback extends HttpCallback {
    function run($request, $response)
    {
        $response->body = "content from php";
    }
}

$callback = new PhpCallback();

$s=new HttpServer();
$s->on('request', $callback);
$s->listen(8082);
$s->start();

```

or in python:
```
import ReactHandler

class PythonCallback(ReactHandler.HttpCallback):
    def run(self, request, response):
            response.body = 'content from python'

callback = PythonCallback()

s = ReactHandler.HttpServer()
s.on('request', callback)
s.listen(8082)
s.start()
```

or in c++ directly:
```
#include "ReactHandler.h"

int main()
{
	HttpServer server;

    server.on<Http::ON_REQUEST>(Utils::to_function([&server](HttpRequest& request, HttpResponse& response) {
		response.body = "content from c++\r\n";
	}));

	server.listen(8082);
	server.start();

	return 0;
}
```

`websocket` server in php:

```
<?php
require("ReactHandler.php");
class PhpCallback extends WebSocketCallback {
    function run($m, $ws)
    {
        if ($m->opcode == WebSocket::OPCODE_TEXT_FRAME) $ws->send($m->client, $m->payload);
    }
}

$callback = new PhpCallback();

$s=new WebSocketServer();
$s->on('message', $callback);
$s->listen(8082);
$s->start();
```

or in python:

```
import ReactHandler

class PythonCallback(ReactHandler.WebSocketCallback):
    def run(self, m, ws):
            if m.opcode == ReactHandler.WebSocket.OPCODE_TEXT_FRAME:
                ws.send(m.client, m.payload)

callback = PythonCallback()

s = ReactHandler.WebSocketServer()
s.on('message', callback)
s.listen(8082)
s.start()

```

or in c++ directly:
```
#include "ReactHandler.h"

int main()
{
	WebSocketServer s;

	s.on<WebSocket::ON_MESSAGE>(Utils::to_function([](WebSocketMessage& m, WebSocket& ws) {
        if (m.opcode == ws.OPCODE_TEXT_FRAME) ws.send(m.client, m.payload);
	}));

	s.listen(8082);
	s.start();
	return 0;
}
```

`socks` server in php:
```
<?php
require("ReactHandler.php");

$s=new SocksServer();
$s->listen(8082);
$s->start();
```

or in python:
```
import ReactHandler

s = ReactHandler.SocksServer()
s.listen(8082)
s.start()

```

or in c++ directly:
```
#include "ReactHandler.h"

int main()
{
	SocksServer server;
	server.listen(8082);
	server.start();

	return 0;
}
```

### Try out
```
#install swig-3.0.8 cmake openssl g++-5.2.1 manually
git clone https://github.com/zhiqiangxu/universe
cd universe/ideas/procpp/mixin_handler
./build.sh

python tests/test.py
php tests/test.php # add build/php/_ReactHandler.so to php.ini manually
python tests/ws.py
php tests/ws.php
python tests/socks.py
php tests/socks.php
```

##Advanced feature

You can choose 3 worker modes:

- Base mode:  master and worker in a **single** process.
- Multiple process mode: master dispatches request to multiple worker processes in the **same** host.
- Remote worker mode: master dispatches request to multiple worker processes in **multiple** host.

## Feedback
- email：<652732310@qq.com>


