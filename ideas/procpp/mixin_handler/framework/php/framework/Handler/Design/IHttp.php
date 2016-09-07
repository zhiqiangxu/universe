<?php
namespace Handler\Design;
use Handler\Design\IProtocol;

interface IHttp extends IProtocol
{

    //fastcgi模式
    static function runMVC();

}
