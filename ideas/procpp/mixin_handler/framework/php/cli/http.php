<?php
require __DIR__ . "/../init.php";


class PhpCallback extends HttpCallback {
    function run($req, $resp)
    {
        $start = microtime(true);
        Handler::getInstance()->handleHttp($req, $resp);
        echo "took " . (microtime(true) - $start) . " seconds\n";
    }
}

$callback = new PhpCallback();

$s=new HttpServer(8082);
//$s->daemonize();
$s->on('request', $callback);
$s->set_worker_num(4);
$s->event_loop();

