<?php
require(__DIR__ . "/../build/php/ReactHandler.php");
class PhpCallback extends HttpCallback {
    function run($req, $resp)
    {
        $resp->body = "content from php";
    }
}

$callback = new PhpCallback();


$s=new HttpServer(8082);
$s->on('request', $callback);
$s->set_worker_num(4);
$s->event_loop();

