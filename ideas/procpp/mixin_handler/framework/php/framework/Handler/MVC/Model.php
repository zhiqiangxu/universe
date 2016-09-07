<?php
namespace Handler\MVC;
use Handler\Design\IModel;
use Handler\Component\Utility;
use Handler\Component\Cache;
use Handler\ShardManager\MySQL as MySQLShardManager;



class Model implements IModel
{

    const QUERY_CACHE_EXPIRE = 5*60;

    function find($parameters)
    {

        if (!empty($parameters['query_cache'])) {

            $cache_key = $this->getQueryCacheKey('find', $parameters);
            $cache = Cache::getInstance()->get($cache_key);

            if ( !($cache === false || is_null($cache)) ) return $cache;

        }

        $shards = $this->getShardsForParameters($parameters);

        $tables = $this->getTablesForParameters($parameters);

        $slave = empty($parameters['master']) ? true : false;

        $result = [];
        foreach ($shards as $shard) {

            $db = $slave ? $shard->getSlave() : $shard->getMaster();
            $shard_result = $db->find($tables, $parameters);
            if ($shard_result) $result = array_merge($result, $shard_result);

        }

        if (!empty($parameters['query_cache'])) {

            Cache::getInstance()->set($cache_key, $result, $this->getQueryCacheExpire($parameters));

        }

        return $result;

    }

    private function getQueryCacheKey($type, $parameters)
    {

        $query_cache = $parameters['query_cache'];

        if (is_string($query_cache)) return $query_cache;

        if (isset($query_cache['key'])) return $query_cache['key'];

        return get_class($this) . '_' . md5("$type:" . json_encode($parameters));

    }

    private function getQueryCacheExpire($parameters)
    {

        $query_cache = $parameters['query_cache'];

        if (is_int($query_cache)) return $query_cache;

        if (isset($query_cache['expire'])) return $query_cache['expire'];

        return static::QUERY_CACHE_EXPIRE;

    }

    private function getShardsForParameters($parameters)
    {

        if (!isset($parameters['shard_id'])) {
            $shards = $this->getAllShards();
        } else {
            $shards = [ $this->getShard($parameters['shard_id']) ];
        }

        return $shards;

    }

    private function getTablesForParameters($parameters)
    {

        $tables = $this->tableNames(isset($parameters['table_id']) ? $parameters['table_id'] : null);

        return is_array($tables) ? $tables : [$tables];

    }

    function get($parameters)
    {

        $parameters['limit'] = 1;
        $result = $this->find($parameters);

        return $result ? $result[0] : $result;

    }

    function count($parameters)
    {

        if (!empty($parameters['query_cache'])) {

            $cache_key = $this->getQueryCacheKey('count', $parameters);
            $cache = Cache::getInstance()->get($cache_key);

            if ( !($cache === false || is_null($cache)) ) return $cache;

        }

        $shards = $this->getShardsForParameters($parameters);

        $tables = $this->getTablesForParameters($parameters);

        $slave = empty($parameters['master']) ? true : false;

        $count = 0;
        foreach ($shards as $shard) {

            $db = $slave ? $shard->getSlave() : $shard->getMaster();
            $shard_count = $db->count($tables, $parameters);
            if ($shard_count) $count += $shard_count;

        }

        if (!empty($parameters['query_cache'])) {
            Cache::getInstance()->set($cache_key, $count, $this->getQueryCacheExpire($parameters));
        }

        return $count;

    }

    function delete($parameters)
    {

        $shards = $this->getShardsForParameters($parameters);

        $tables = $this->getTablesForParameters($parameters);

        foreach ($shards as $shard) {

            if (!$shard->getMaster()->delete($tables, $parameters)) return false;

        }

        return true;

    }

    function insert($data, $parameters = [])
    {

        $shards = $this->getShardsForParameters($parameters);

        $tables = $this->getTablesForParameters($parameters);

        $return_id = empty($parameters['return_id']) ? false : true;
        $id_name = empty($parameters['id_name']) ? null : $parameters['id_name'];

        $insert_ids = [];
        foreach ($shards as $shard) {

            $result = $shard->getMaster()->insert($tables, $data, $return_id, $id_name);
            if (!$result) return false;
            if ($return_id) $insert_ids = array_merge($insert_ids, $result);

        }

        return $return_id ? $insert_ids : true;

    }

    function update($parameters, $update_data)
    {

        $shards = $this->getShardsForParameters($parameters);

        $tables = $this->getTablesForParameters($parameters);

        foreach ($shards as $shard) {

            if (!$shard->getMaster()->update($tables, $parameters, $update_data)) return false;

        }

        return true;

    }

    static private $_table_id_map = [];
    static private $_all_shard_tables = [];

    // 获取表名,可以是多个
    function tableNames($table_id = null)
    {

        //单表
        if (defined('static::TABLE')) return static::TABLE;

        if (!defined('static::SHARD_TABLE')) {

            $cls = get_called_class();

            $seperator_rindex = strrpos($cls, '\\');

            if ($seperator_rindex === false) $class_name = $cls;
            else $class_name = substr($cls, $seperator_rindex + 1);

            return Utility::camel2Snake($class_name);

        }

        //分表
        $php = \Handler::$php;

        if (!self::$_table_id_map) {
            if (!defined('static::SHARD_TABLE_IDS')) $php->logger->fatal("SHARD_TABLE_IDS is required for SHARD_TABLE " . static::SHARD_TABLE);

            foreach (static::SHARD_TABLE_IDS as $table_id) self::$_table_id_map[$table_id] = 1;
        }

        //null means all
        if (is_null($table_id)) {
            if (!self::$_all_shard_tables) self::$_all_shard_tables = array_map(
                function ($table_id) {
                    return self::SHARD_TABLE . '_' . $table_id;
                },
                $static::SHARD_TABLE_IDS
            );

            return self::$_all_shard_tables;
        }

        if (!isset(self::$_table_id_map[$table_id])) $php->logger->fatal("invalid table_id $table_id for SHARD_TABLE " . static::SHARD_TABLE);

        return static::SHARD_TABLE . '_' . $table_id;

    }

    // 获取shard
    function getShard($shard_id)
    {

        $options = ['shard_id' => $shard_id];

        if (method_exists($this, 'shardIdCmp')) $options['shard_id_cmp'] = array($this, 'shardIdCmp');

        return MySQLShardManager::getInstance()->getShard(static::DB_GROUP, $options);

    }

    function getAllShards()
    {

        return MySQLShardManager::getInstance()->getAllShards(static::DB_GROUP);

    }

}
