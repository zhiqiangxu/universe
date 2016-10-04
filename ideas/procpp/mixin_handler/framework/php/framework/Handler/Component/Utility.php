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

    static function generateRandomString($length = 10)
    {
        $characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
        $charactersLength = strlen($characters);
        $randomString = '';
        for ($i = 0; $i < $length; $i++) {
            $randomString .= $characters[rand(0, $charactersLength - 1)];
        }
        return $randomString;
    }
}
