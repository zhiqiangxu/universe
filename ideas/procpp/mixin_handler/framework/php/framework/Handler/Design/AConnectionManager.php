<?php
namespace Handler\Design;

abstract class AConnectionManager
{

    protected $_client_class;
    protected function __construct()
    {

        $class_name = get_class($this);
        $ms_class = substr($class_name, strrpos($class_name, '\\') + 1);
        $this->_client_class = 'Handler\\Client\\' . $ms_class;

    }

    static private $_instances = [];
    //['host' => xx, 'port' => yy, ...]
    function getConnection($connection_info)
    {

        $singleton_key = sprintf('%s:%s', $connection_info['host'], $connection_info['port']);

        if (!isset(self::$_instances[$singleton_key])) self::$_instances[$singleton_key] = new $this->_client_class($connection_info);

        return self::$_instances[$singleton_key];

    }
}
