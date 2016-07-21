<?php
namespace ReactHandler;

class Object
{
    function __get($key)
    {
        return Handler::$php->$key;
    }

    function __call($func, $param)
    {
        return call_user_func_array([Handler::$php, $func], $param);
    }
}
