<?php
namespace Handler\Design;

interface ILoader
{

    static function autoload($class);
    static function addNameSpace($root, $path);

}
