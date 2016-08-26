<?php
namespace Handler\ConnectionManager;
use Handler\Design\IConnectionManager;
use Handler\Client\MySQL as MySQLClient;

class MySQL implements IConnectionManager
{

    static private $_instances = [];

    static function getConnection($connection_info)
    {

        $singleton_key = sprintf('%s:%s', $connection_info['host'], $connection_info['port']);

        if (!isset(self::$_instances[$singleton_key])) self::$_instances[$singleton_key] = new MySQLClient($connection_info);

        return self::$_instances[$singleton_key];

    }

}
