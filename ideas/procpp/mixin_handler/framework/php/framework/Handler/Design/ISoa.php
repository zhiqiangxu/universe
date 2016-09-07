<?php
namespace Handler\Design;
use Handler\Design\IProtocol;

interface ISoa extends IProtocol
{

    static function pushResponse($session_id, $json);

}
