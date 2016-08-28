<?php
namespace Handler\Component;

use Handler\Design\ILogger;

class Logger implements ILogger
{

    static private $_instance = null;

    static function getInstance()
    {

        if (!self::$_instance) self::$_instance = new Logger;

        return self::$_instance;

    }

    function notice($message)
    {
    }

    function warning($message)
    {
    }

    function fatal($message)
    {

        exit($message);

    }

}
