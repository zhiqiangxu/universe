<?php

namespace ReactHandler;

use Client\MySQL;
use Client\Redis;

class Model extends Object
{
    const DB_STRING = 'DB';
    const REDIS_STRING = 'REDIS';


    // Client系
    function getMysql($slave = false, $sharding_key = null)
    {
        $group = $this->_getClientGroup(self::DB_STRING);

        return MySQL::getInstance($group, $slave, $sharding_key);
    }

    function getRedis($group = null)
    {
        if (!$group) $group = $this->_getClientGroup(self::REDIS_STRING);

        return Redis::getInstance($group);
    }

    function setCache($key, $value, $expire = 3600, $group = 'default')
    {
        $redis = $this->getRedis($group);
        return $redis->setEx($this->_keyPrefix($key), $expire, $value);
    }

    function getCache($key, $group = 'default')
    {
        $redis = $this->getRedis($group);
        return $redis->get($this->_keyPrefix($key));
    }

    private function _constant($name, $default = null)
    {
        $constant_name = get_class($this) . '::' . $name;
        return defined($constant_name) ? constant($constant_name) : $default;
    }

    private function _getClientGroup($client_type, $default = 'default')
    {
        return $this->_constant($client_type . "_GROUP", $default);
    }

    private function _keyPrefix($key)
    {
        return get_called_class() . ":$key";
    }
}
