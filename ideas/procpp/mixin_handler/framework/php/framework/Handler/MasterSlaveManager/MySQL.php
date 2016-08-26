<?php
namespace Handler\MasterSlaveManager;
use Handler\Design\AMasterSlaveManager;
use Handler\ConnectionManager\MySQL as MySQLConnectionManager;

class MySQL extends AMasterSlaveManager
{

    static private $_instances = null;

    //['host' => xx, 'port' => yy, ...]
    static function getInstance($ms_info)
    {

        $singleton_key = sprintf('%s:%s', $ms_info['host'], $ms_info['port']);

        if (!isset(self::$_instances[$singleton_key])) self::$_instances[$singleton_key] = new MySQL($ms_info);

        return self::$_instances[$singleton_key];

    }

    function getMaster()
    {

        return MySQLConnectionManager::getConnection($this->_mst_info);

    }

    function getSlave()
    {

        $slave_info = $this->_hasher->lookupTarget(rand());

        if (!$slave_info) {
            //如未配slave，则返回master
            return $this->getMaster();
        }

        return MySQLConnectionManager::getConnection($slave_info);

    }

}
