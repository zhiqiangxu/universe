<?php
namespace Handler\Design;

interface IConnectionManager
{
    //['host' => xx, 'port' => yy, ...]
    static function getConnection($connection_info);
}
