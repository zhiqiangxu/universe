<?php
namespace Handler\Design;

interface IHandler
{

    static function getInstance();
    //cli
    function handleHttp($request, $response);
    function handleSoa($request, $response);
    function setServer($server);
    function getSessionId($request);
    function pushMessage($session_id, $string);
    function pushSoaResponse($session_id, $json);

}
