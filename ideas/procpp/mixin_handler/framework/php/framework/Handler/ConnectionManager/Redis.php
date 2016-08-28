<?php
namespace Handler\ConnectionManager;
use Handler\Design\AConnectionManager;

class Redis extends AConnectionManager
{

    private static $_instance;
    static function getInstance()
    {

        if (!self::$_instance) self::$_instance = new Redis;

        return self::$_instance;

    }

    protected function __construct()
    {

        parent::__construct();

    }

}
