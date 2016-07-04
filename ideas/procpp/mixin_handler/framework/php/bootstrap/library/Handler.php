<?php

use Handler\Http;

class Handler
{
    public $request;
    public $response;

    static private $instance;

    static public function getInstance()
    {
        if (!self::$instance) self::$instance = new Handler();

        return self::$instance;
    }

    public function dispatchHttp($request, $response)
    {
        try {
            echo "dispatchHttp\n";
            Http::handle($request, $response);
        } catch (Exception $e) {
            var_dump($e);
        }
    }

}


