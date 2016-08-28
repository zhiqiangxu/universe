<?php
namespace Handler\ConnectionManager;
use Handler\Design\AConnectionManager;
use Handler\Client\MySQL as MySQLClient;

class MySQL extends AConnectionManager
{

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
