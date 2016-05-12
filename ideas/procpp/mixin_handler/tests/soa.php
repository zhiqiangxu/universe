<?php
require(__DIR__ . "/../build/php/ReactHandler.php");
class PhpCallback extends SoaCallback {
    function run($req, $resp)
    {
        var_dump(json_decode($req->json, true));
        $resp->json = json_encode(['key' => 'value']);
        sleep(10);
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
$s->on('request', $callback);
$s->dispatch(4);
$s->listen(8082);
$s->start();

