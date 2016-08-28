<?php
namespace Handler\Component;
use Handler\Design\IUtility;

class Utility implements IUtility
{

    static function camel2Snake($name)
    {

        return ltrim(strtolower(preg_replace('/[A-Z]/', '_$0', $name)), '_');

    }

    static function snake2Camel($name)
    {

        return str_replace(' ', '', ucwords(str_replace('_', ' ', $name)));

    }

    static function array_rand_elements($array, $n = 1)
    {
        $keys = array_rand($array, $n);
        if (!is_array($keys)) return $array[$keys];

        $elements = [];
        foreach ($keys as $idx) {
            $elements[] = $array[$idx];
        }
        return $elements;
    }

}
