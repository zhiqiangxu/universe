<?php
namespace Handler\ShardManager;
use Handler\Design\IShardManager;
use Handler\MasterSlaveManager\MySQL as MasterSlaveManager;

class MySQL implements IShardManager
{


    static function getShard($group, $options = [])
    {

        $php = \Handler::$php;
        $conf = $php->config['mysql'];

        if (!isset($conf[$group])) {
            $php->logger->fatal("no group $group for mysql");
        }

        $group_conf = $conf[$group];

        $ms_info = null;

        if (isset($group_conf[0])) {

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

        } else $ms_info = $group_conf;

        return MasterSlaveManager::getInstance($ms_info);

    }

    static function getAllShards($group)
    {

        $php = \Handler::$php;
        $conf = $php->config['mysql'];

        if (!isset($conf[$group])) {
            $php->logger->fatal("no group $group for mysql");
        }

        $group_conf = $conf[$group];
        if (isset($group_conf[0])) {

            $all = [];

            foreach ($group_conf as $shard_info) $all[] = MasterSlaveManager::getInstance($shard_info);

            return $all;

        } else return [ MasterSlaveManager::getInstance($group_conf) ];

    }

}
