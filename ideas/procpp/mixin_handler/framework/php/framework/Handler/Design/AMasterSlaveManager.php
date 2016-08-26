<?php
namespace Handler\Design;
use Handler\Component\ConsistentHash;

abstract class AMasterSlaveManager
{

    protected $_hasher;
    protected $_mst_info;

    //只能通过getInstance构造
    protected function __construct($ms_info)
    {

        $this->_hasher = new ConsistentHash;

        $this->_mst_info = $ms_info;

        $slave_infos = empty($ms_info['slaves']) ? [] : $ms_info['slaves'];
        foreach ($slave_infos as $slave_info) {
            $this->addSlaveInfo($slave_info);
        }


    }

    function getShardId()
    {

        return isset($this->_mst_info['shard_id']) ? $this->_mst_info['shard_id'] : null;

    }

    abstract function getMaster();

    //每次随机，如果需要返回同一个实例，调用端缓存
    abstract function getSlave();

    function addSlaveInfo($weighted_connection_info)
    {

        $weight = isset($weighted_connection_info['weight']) ? $weighted_connection_info['weight'] : 1;

        $this->_hasher->addTarget($weighted_connection_info, $weight);

    }


}
