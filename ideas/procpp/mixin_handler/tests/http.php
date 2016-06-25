<?php
require(__DIR__ . "/../build/php/ReactHandler.php");
class PhpCallback extends HttpCallback {
    function run($req, $resp)
    {
        $resp->body = "content from php";
    }
}

$callback = new PhpCallback();

/*
$s=new SoaServer();
$s->on('request', $callback);
$s->listen(8082);
$s->start();
*/

$s=new HttpProcessDispatcherServer();
$s->on('request', $callback);
$s->listen(8082);
$s->dispatch(4);

