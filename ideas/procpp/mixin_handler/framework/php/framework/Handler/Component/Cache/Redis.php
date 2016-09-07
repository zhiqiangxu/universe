<?php
namespace Handler\Component\Cache;

use Handler\ConnectionManager\Redis as RedisConnectionManager;

use Handler\ShardManager\Redis as RedisShardManager;

use Handler\Design\ICache;

use Handler\Component\ConsistentHash;

class Redis implements ICache
{
    protected $_hasher;


    static $_instances = [];
    static function getInstance($group)
    {

        if (!isset(self::$_instances[$group])) self::$_instances[$group] = new Redis($group);

        return self::$_instances[$group];
    }

    private function __construct($group)
    {

        $php = \Handler::$php;
        $conf = $php->config[RedisShardManager::CONF];
        if (!isset($conf[$group])) exit("no $group in conf " . RedisShardManager::CONF);
        $group_conf = $conf[$group];

        $this->_hasher = new ConsistentHash;

        if (isset($group_conf[0])) {
            foreach ($group_conf as $node_info) {
                // TODO handle weight in one place
                $this->_hasher->addTarget($node_info, isset($node_info['weight']) ? $node_info['weight'] : 1);
            }
        } else $this->_hasher->addTarget($group_conf, isset($group_conf['weight']) ? $group_conf['weight'] : 1);

    }

    function set($key, $data, $expire)
    {

        $connection_info = $this->_hasher->lookupTarget($key);
        $client = RedisConnectionManager::getInstance()->getConnection($connection_info);
        return $client->setEx($key, serialize($data), $expire);

    }

    function get($key, $unserialize = true)
    {

        $connection_info = $this->_hasher->lookupTarget($key);
        $client = RedisConnectionManager::getInstance()->getConnection($connection_info);
        $result = $client->get($key);
        if ($result && $unserialize) $result = unserialize($result);

        return $result;

    }

}
