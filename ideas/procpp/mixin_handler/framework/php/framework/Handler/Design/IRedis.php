<?php
namespace Handler\Design;

interface IRedis
{
    //kv
    function set($key, $data);
    function get($key);
    function del($key);
    function setNx($key, $data);
    function setEx($key, $data, $expire);
    function setNxEx($key, $data, $expire);
    function incr($key, $by);
    function decr($key, $by);

    //hash
    function hSet($key, $attr, $value);
    function hGet($key, $attr);
    function hGetAll($key);

}
