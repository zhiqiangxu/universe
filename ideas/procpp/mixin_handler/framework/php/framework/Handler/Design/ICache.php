<?php
namespace Handler\Design;

interface ICache
{
    function set($key, $data, $expire);

    function get($key);
}


