# Reactor server framework for mixin programing


**mixin handler** offers two fundamental features:
 
- a **generic** server framework that follows react pattern, written in c++；
- you can use it in various scripting languages such as php/python and more；

It is **generic** in that it can be used for all kinds of servers, such as socks server, http server, etc.

All you need to do to support another kind of server is to offer a class that extends `Protocol`(tcp) or `UProtocol`(udp).


-------------------

## Demo

A  http server can be writen in php: 

``` <?php
require("ReactHandler.php");
class PhpCallback extends HttpRequestCallback {
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

class PythonCallback(ReactHandler.HttpRequestCallback):
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
		response.body = "additional content from hook\r\n";
	}));

	server.listen(8082);
	server.start();

	return 0;
}
```

### Try out
```
git clone https://github.com/zhiqiangxu/universe
cd universe/ideas/procpp/mixin_handler
./build.sh

python tests/test.py
php tests/test.php # add build/php/_ReactHandler.so to php.ini manually
```

##Advanced feature

You can choose 3 worker modes:

- Base mode:  master and worker in a **single** process.
- Multiple process mode: master dispatches request to multiple worker processes in the **same** host.
- Remote worker mode: master dispatches request to multiple worker processes in **multiple** host.

## Feedback
- email：<652732310@qq.com>


