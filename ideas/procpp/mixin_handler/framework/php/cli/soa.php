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

/*
$s=new SoaServer();
$s->on('request', $callback);
$s->listen(8082);
$s->start();
*/

$s=new SoaProcessDispatcherServer();
Handler::$php->setServer($s);
//$s->daemonize();
$s->on('request', $callback);
$s->dispatch(4);
$s->listen(8083);
$s->start();

