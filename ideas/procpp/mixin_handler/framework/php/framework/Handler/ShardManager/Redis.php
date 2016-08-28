<?php
namespace Handler\ShardManager;
use Handler\Design\AShardManager;
use Handler\MasterSlaveManager\Redis as RedisMasterSlaveManager;

class Redis extends AShardManager
{

    const CONF = 'redis';

    private static $_instance;
    static function getInstance()
    {

        if (!self::$_instance) self::$_instance = new MySQL;

        return self::$_instance;

    }

    protected function __construct()
    {

        parent::__construct();

    }


}
