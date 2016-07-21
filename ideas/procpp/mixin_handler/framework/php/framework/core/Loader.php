<?php

namespace ReactHandler;

class Loader
{
    static $namespaces = [];

    static function autoload($class)
    {
        $root = explode('\\', trim($class, '\\'), 2);

        if (count($root) > 1 and isset(self::$namespaces[$root[0]])) {
            include self::$namespaces[$root[0]] . DIRECTORY_SEPARATOR . str_replace('\\', DIRECTORY_SEPARATOR, $root[1]).'.php';
        }
    }

    static function addNameSpace($root, $path)
    {
        self::$namespaces[$root] = $path;
    }
}
