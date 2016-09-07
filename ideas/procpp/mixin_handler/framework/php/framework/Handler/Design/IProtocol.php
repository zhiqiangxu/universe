<?php
namespace Handler\Design;

interface IProtocol
{

    //用于CLI模式
    static function handle($request, $response);

}
