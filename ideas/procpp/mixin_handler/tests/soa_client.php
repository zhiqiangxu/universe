<?php
require(__DIR__ . "/../build/php/ReactHandler.php");
class PhpCallback extends SoaClientCallback {
    function run($json)
    {
        var_dump(json_decode($json, true));
    }
}

$callback = new PhpCallback();

$c=new SoaClient("localhost", 8082);
$request_id1 = $c->call(json_encode(['client_key' => 'client_value']), $callback);
$request_id2 = $c->call(json_encode(['client_key2' => 'client_value2']), $callback);

var_dump($c->wait(50, [$request_id1, $request_id2]));

