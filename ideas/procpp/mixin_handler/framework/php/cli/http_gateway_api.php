<?php
require __DIR__ . "/../init.php";


class PhpCallback extends HttpGatewayCallback {
    function on_request_uri($request, $provider_address, $result)
    {

        $uri_info = parse_url($request->uri);
        $path = $uri_info['path'];

        $result->ok = true;
        $provider_address->host = 'api.ffan.com';
        $provider_address->port = 80;

        $provider_address->path = $path;

    }

    function on_response($request, $response)
    {

        $uri_info = parse_url($request->uri);
        $path = $uri_info['path'];

    }
}

$callback = new PhpCallback();

$s=new HttpGatewayServer(8082);
//$s->daemonize();
$s->register_callback($callback);
$s->set_worker_num(4);

$s->event_loop();
