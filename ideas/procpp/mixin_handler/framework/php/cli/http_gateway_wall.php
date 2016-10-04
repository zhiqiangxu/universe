<?php
require __DIR__ . "/../init.php";

//TODO handle https

class PhpCallback extends HttpGatewayCallback {

    const TARGET_DOMAIN = 'www.baidu.com';

    function on_request_uri($request, $provider_address, $result)
    {

        $uri_info = parse_url($request->uri);
        $path = $uri_info['path'];

        $result->ok = true;
        $provider_address->host = self::TARGET_DOMAIN;
        $provider_address->port = 443;
        $provider_address->ssl = true;

        $provider_address->path = $path;

    }

    function on_response($request, $response)
    {

        $domain = 'http://test.com:8082';
        $response->body = str_replace('https://' . self::TARGET_DOMAIN, $domain, str_replace('http://' . self::TARGET_DOMAIN, $domain, $response->body));

    }
}

$callback = new PhpCallback();

$s=new HttpGatewayServer(8082);
//$s->daemonize();
$s->register_callback($callback);
$s->set_worker_num(4);
$s->event_loop();

