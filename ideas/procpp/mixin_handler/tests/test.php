<?php
require(__DIR__ . "/../build/php/ReactHandler.php");
class PhpCallback extends HttpRequestCallback {
    function run($req, $resp)
    {
        $resp->body = "content from php";
    }
}

$callback = new PhpCallback();
$callback->thisown = 0;

$s=new HttpServer();
$s->on('request', $callback);
$s->listen(8082);
$s->start();
