<?php
require(__DIR__ . "/../init.php");
class PhpCallback extends SoaClientCallback {
    function run($json)
    {
        var_dump(json_decode($json, true));
    }
}

$callback = new PhpCallback();

$c=new SoaClient("localhost", 8083);
$c->register_push_callback($callback);
//$request_id1 = $c->call(json_encode(['method' => 'Test::test']), $callback);
$request_id1 = $c->call(json_encode(['method' => 'Test::test']));

//var_dump($c->wait([$request_id1, $request_id2], 25000));

while (true) {
    $c->wait_n(1, 50);
}
