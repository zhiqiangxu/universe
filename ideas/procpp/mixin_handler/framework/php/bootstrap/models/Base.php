<?php

namespace Model;

use Client\MySQL;
use Client\Redis;

class Base
{
    static function getInstance()
    {
        static $instance = [];
        $class = get_called_class();

        if (!isset($instance[$class])) $instance[$class] = new $class();

        return $instance[$class];
    }

    function getMysql($group = 'default', $slave = false, $sharding_key = null)
    {
        return MySQL::getInstance($group, $slave, $sharding_key);
    }

    function getRedis($group = 'default')
    {
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

    private function _keyPrefix($key)
    {
        return get_called_class() . ":$key";
    }
}
