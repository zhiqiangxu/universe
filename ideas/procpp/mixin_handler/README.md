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

$s=new HttpServer(8082);
$s->on('request', $callback);
$s->event_loop();

```

or in python:
```
import ReactHandler

class PythonCallback(ReactHandler.HttpCallback):
    def run(self, request, response):
            response.body = 'content from python'

callback = PythonCallback()

s = ReactHandler.HttpServer(8082)
s.on('request', callback)
s.event_loop()
```

or in c++ directly:
```
#include "ReactHandler.h"

int main()
{
    HttpServer server(8082);

    server.on<Http::ON_REQUEST>(Utils::to_function([&server](HttpRequest& request, HttpResponse& response) {
        response.body = "content from c++\r\n";
    }));

    server.event_loop();

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

$s=new WebSocketServer(8082);
$s->on('message', $callback);
$s->event_loop();
```

or in python:

```
import ReactHandler

class PythonCallback(ReactHandler.WebSocketCallback):
    def run(self, m, ws):
            if m.opcode == ReactHandler.WebSocket.OPCODE_TEXT_FRAME:
                ws.send(m.client, m.payload)

callback = PythonCallback()

s = ReactHandler.WebSocketServer(8082)
s.on('message', callback)
s.event_loop()

```

or in c++ directly:
```
#include "ReactHandler.h"

int main()
{
    WebSocketServer s(8082);

    s.on<WebSocket::ON_MESSAGE>(Utils::to_function([](WebSocketMessage& m, WebSocket& ws) {
        if (m.opcode == ws.OPCODE_TEXT_FRAME) ws.send(m.client, m.payload);
    }));

    s.event_loop();
    return 0;
}
```

`socks` server in php:
```
<?php
require("ReactHandler.php");

$s=new SocksServer(8082);
$s->event_loop();
```

or in python:
```
import ReactHandler

s = ReactHandler.SocksServer(8082)
s.event_loop()

```

or in c++ directly:
```
#include "ReactHandler.h"

int main()
{
    SocksServer server(8082);
    server.event_loop();

    return 0;
}
```

### Try out
```
#install swig-3.0.8 cmake-3.4.1 openssl g++-5.2.1 manually
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

You can choose 2 worker modes:

- Base mode:  single process mode.
- Multiple process mode.

## Feedback
- email：<652732310@qq.com>


