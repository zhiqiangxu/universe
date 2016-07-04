<?php

namespace Utils;

class Common
{
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
