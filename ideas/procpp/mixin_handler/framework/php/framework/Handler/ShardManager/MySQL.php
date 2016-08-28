<?php
namespace Handler\ShardManager;
use Handler\Design\AShardManager;

class MySQL extends AShardManager
{

    const CONF = 'mysql';

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
