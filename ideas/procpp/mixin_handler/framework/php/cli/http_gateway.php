<?php
require __DIR__ . "/../init.php";


class PhpCallback extends HttpGatewayCallback {
    function on_request_uri($request, $provider_address, $result)
    {
        $result->ok = true;
        $provider_address->host = 'api.sit.ffan.com';
        $provider_address->port = 80;
        $provider_address->path = $request->uri;
    }
}

$callback = new PhpCallback();

$s=new HttpGatewayProcessDispatcherServer();
//$s->daemonize();
$s->register_callback($callback);
$s->listen(8082);
$s->dispatch(4);

