<?php
namespace Handler\MVC\Soa;
use Handler\Design\IController;

class Controller implements IController
{

    function outputJson($json)
    {

        global $php;
        $php->response->json = json_encode($json);

    }

    function outputText($text)
    {
        exit('NOT SUPPORTED');
    }

    function header()
    {
        exit('NOT SUPPORTED');
    }

    function assign()
    {
        exit('NOT SUPPORTED');
    }

    function fetch($tpl_file)
    {
        exit('NOT SUPPORTED');
    }

    function outputHtml($tpl_file)
    {
        exit('NOT SUPPORTED');
    }

    function setCookie($name, $value = '', $expire = 0, $path = '', $domain = '', $secure = false, $httponly = false)
    {
        exit('NOT SUPPORTED');
    }

    function sessionStart()
    {
    }
}
