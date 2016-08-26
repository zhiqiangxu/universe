<?php
namespace Handler\Design;

interface IHttp
{

    //用于CLI模式
    static function handle($request, $response);
    //用于fastcgi模式
    static function runMVC();


}
