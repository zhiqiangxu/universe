<?php
namespace Handler\Design;

abstract class AShardManager
{

    protected $_target_method;

    protected function __construct()
    {

        $class_name = get_class($this);
        $ms_class = substr($class_name, strrpos($class_name, '\\') + 1);
        $this->_target_method = ['Handler\\MasterSlaveManager\\' . $ms_class, 'getInstance'];

    }

    //获取单个shard
    function getShard($group, $options = [])
    {

        $type = static::CONF;

        $php = \Handler::$php;
        $conf = $php->config[$type];

        if (!isset($conf[$group])) {
            $php->logger->fatal("no group $group for $type");
        }

        $group_conf = $conf[$group];

        $ms_info = null;

        if (isset($group_conf[0]) ) {

            if (count($group_conf) > 1) {
                //明确指定shard_id
                if (isset($options['shard_id'])) {

                    $custom_shard_id_compare = isset($options['shard_id_cmp']) ? $options['shard_id_cmp'] : null;
                    foreach ($group_conf as $shard_info) {
                        if ($custom_shard_id_compare ? $custom_shard_id_compare($shard_info['shard_id'], $options['shard_id']) : $shard_info['shard_id'] == $options['shard_id']) {
                            $ms_info = $shard_info;
                            break;
                        }
                    }

                    if (!$ms_info) $php->logger->fatal("no matching shard for sharding $group:{$options['shard_id']}");

                } else $php->logger->fatal("no shard_id specified for sharding group $group");

            } else $ms_info = $group_conf[0];

        } else $ms_info = $group_conf;

        return call_user_func($this->_target_method, $ms_info);

    }

    //获取所有shard
    function getAllShards($group)
    {

        $type = static::CONF;

        $php = \Handler::$php;
        $conf = $php->config[$type];

        if (!isset($conf[$group])) {
            $php->logger->fatal("no group $group for $type");
        }

        $group_conf = $conf[$group];
        if (isset($group_conf[0])) {

            $all = [];

            foreach ($group_conf as $shard_info) $all[] = call_user_func($this->_target_method, $shard_info);

            return $all;

        } else return [ call_user_func($this->_target_method, $group_conf) ];

    }

}
