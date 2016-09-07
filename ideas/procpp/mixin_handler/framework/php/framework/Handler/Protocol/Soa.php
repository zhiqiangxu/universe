<?php
namespace Handler\Protocol;
use Handler\Design\ISoa;

class Soa implements ISoa
{

    static function handle($request, $response)
    {

        global $php;
        $php->request = $request;
        $php->response = $response;

        $json = json_decode($request->json, true);

        if (!$json) return;

        $method = $json['method'];
        list ($controller, $action) = explode('::', $method);

        if (isset($json['args'])) {

            $args = $json['args'];
            if (!isset($args[0])) $args = [$args];

        } else $args = [];

        $controller_class = '\\App\\Controller\\Soa\\' . $controller;
        $controller_instance = new $controller_class;
        return call_user_func_array([$controller_instance, $action], $args);

    }

    static function pushResponse($session_id, $json)
    {

        $soa_response = new \SoaResponse();
        $soa_response->json = json_encode($json);

        return \Handler::$php->pushMessage($session_id, $soa_response->to_string());

    }

}
