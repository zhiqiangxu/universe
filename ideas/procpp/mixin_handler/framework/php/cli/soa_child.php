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

$s=new SoaServer(8084);
$s->set_parent('127.0.0.1', 8085);
Handler::$php->setServer($s);
//$s->daemonize();
$s->on('request', $callback);
$s->event_loop();

