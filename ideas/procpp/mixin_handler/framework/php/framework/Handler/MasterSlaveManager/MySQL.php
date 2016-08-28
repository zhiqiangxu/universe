<?php
namespace Handler\MasterSlaveManager;
use Handler\Design\AMasterSlaveManager;

class MySQL extends AMasterSlaveManager
{

    static private $_instances = null;

    //['host' => xx, 'port' => yy, ...]
    static function getInstance($ms_info)
    {

        $singleton_key = sprintf('%s:%s', $ms_info['host'], $ms_info['port']);

        if (!isset(self::$_instances[$singleton_key])) self::$_instances[$singleton_key] = new MySQL($ms_info);

        return self::$_instances[$singleton_key];

    }

}
