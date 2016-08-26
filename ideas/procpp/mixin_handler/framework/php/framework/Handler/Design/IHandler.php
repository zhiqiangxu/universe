<?php
namespace Handler\Design;

interface IHandler
{

    static function getInstance();
    //cli
    function dispatchHttp($request, $response);

}
