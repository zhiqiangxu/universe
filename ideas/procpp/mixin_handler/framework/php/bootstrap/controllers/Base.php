<?php
namespace Controller;

class Base
{

    public function outputJson($json)
    {

        global $php;
        $this->header('Content-Type', 'application/json; charset=utf-8');
        $php->response->body = json_encode($json);

    }

    public function outputText($text)
    {

        global $php;
        $this->header('Content-Type', 'text/plain; charset=utf-8');
        $php->response->body = $text;

    }

    public function header($name, $value)
    {

        global $php;
        $response = $php->response;
        $response->headers->set($name, $value);

    }
}
