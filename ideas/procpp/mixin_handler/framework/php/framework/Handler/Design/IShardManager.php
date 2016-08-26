<?php
namespace Handler\Design;

interface IShardManager
{

    //获取单个shard
    static function getShard($group, $options = []);

    //获取所有shard
    static function getAllShards($group);

}
