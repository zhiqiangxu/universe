<?php
namespace Handler\Design;
use Handler\Component\ConsistentHash;

abstract class AMasterSlaveManager
{

    protected $_hasher;
    protected $_mst_info;
    protected $_connection_manager;

    //只能通过getInstance构造
    protected function __construct($ms_info)
    {

        $this->_hasher = new ConsistentHash;

        $this->_mst_info = $ms_info;

        $slave_infos = empty($ms_info['slaves']) ? [] : $ms_info['slaves'];
        foreach ($slave_infos as $slave_info) {
            $this->addSlaveInfo($slave_info);
        }

        $class_name = get_class($this);
        $ms_class = substr($class_name, strrpos($class_name, '\\') + 1);
        $this->_connection_manager = call_user_func(['Handler\\ConnectionManager\\' . $ms_class, 'getInstance']);


    }

    function getShardId()
    {

        return isset($this->_mst_info['shard_id']) ? $this->_mst_info['shard_id'] : null;

    }

    function getMaster()
    {

        return $this->_connection_manager->getConnection($this->_mst_info);

    }

    //每次随机，如果需要返回同一个实例，调用端缓存
    function getSlave()
    {

        $slave_info = $this->_hasher->lookupTarget(rand());

        if (!$slave_info) {
            //如未配slave，则返回master
            return $this->getMaster();
        }

        return $this->_connection_manager->getConnection($slave_info);

    }

    function addSlaveInfo($weighted_connection_info)
    {

        $weight = isset($weighted_connection_info['weight']) ? $weighted_connection_info['weight'] : 1;

        $this->_hasher->addTarget($weighted_connection_info, $weight);

    }


}
