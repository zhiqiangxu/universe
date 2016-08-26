<?php
namespace Handler\Design;

interface IUtility
{
    static function camel2Snake($name);
    static function snake2Camel($name);
    static function array_rand_elements($array, $n = 1);
}
