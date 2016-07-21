<?php


namespace ReactHandler\Client;

use \ReactHandler\Common;
use \ReactHandler\ConsistentHash;

class Redis
{
    static private $instances = [];

    private $hasher;
    private $redis_map;

    static function getInstance($group)
    {
        $conf = \ReactHandler::$php->config['redis'];

        if (!isset(self::$instances[$group])) {
            $group_conf = $conf[$group];

            self::$instances[$group] = new Redis($group_conf);
        }

        return self::$instances[$group];
    }

    function __construct($group_conf)
    {
        $this->hasher = new ConsistentHash;
        $this->hasher->addTargets($group_conf);
    }

    function setEx($key, $expire, $value)
    {
        $redis = $this->_getRedis($key);
        return $redis->setEx($key, $expire, serialize($value));
    }

    function get($key)
    {
        $redis = $this->_getRedis($key);
        $result = $redis->get($key);
        if (!$result) return null;

        return unserialize($result);
    }

    function __call($method, $params)
    {
        $key = $params[0];
        return call_user_func_array([$this->_getRedis($key), $method], $params);
    }

    private function _getRedis($key)
    {
        $target = $this->hasher->lookupTarget($key);
        $redis_key = $target['host'] . ':' . $target['port'];
        if (!isset($this->redis_map[$redis_key])) {

            $redis = new \Redis();
            if (!empty($target['pconnect'])) $redis->pconnect($target['host'], $target['port'], $target['timeout']);
            else $redis->connect($target['host'], $target['port'], $target['timeout']);

            $this->redis_map[$redis_key] = $redis;
        }

        return $this->redis_map[$redis_key];
    }
}
