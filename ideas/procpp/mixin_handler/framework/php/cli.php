<?php
require __DIR__ . "/init.php";


class PhpCallback extends HttpCallback {
    function run($req, $resp)
    {
        $start = microtime(true);
        Handler::getInstance()->dispatchHttp($req, $resp);
        echo "took " . (microtime(true) - $start) . " seconds\n";
    }
}

$callback = new PhpCallback();

$s=new HttpProcessDispatcherServer();
$s->on('request', $callback);
$s->listen(8082);
$s->dispatch(1);

