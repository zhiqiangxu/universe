<?php
require __DIR__ . "/../init.php";


class PhpCallback extends SoaCallback {
    function run($req, $resp)
    {
        Handler::$php->handleSoa($req, $resp);
        Handler::$php->pushSoaResponse(Handler::$php->getSessionId($req), 'pushed message');
    }
}

$callback = new PhpCallback();

$s=new SoaServer(8082);
//$s->daemonize();
$s->on('request', $callback);
$s->set_worker_num(4);
$s->event_loop();


