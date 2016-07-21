<?php


namespace ReactHandler\Client;

use \ReactHandler\Common;
use \ReactHandler\ConsistentHash;

class MySQL
{
    private static $instances = [];
    private static $mysql_map;

    private $pdo = null;
    private $slave;

    static function getInstance($group, $slave, $sharding_key)
    {

        $conf = \ReactHandler::$php->config['mysql'];

        if (!isset(self::$instances[$group][$slave])) {

            $group_conf = $conf[$group];

            self::$instances[$group][$slave] = new MySQL($group_conf, $slave, $sharding_key);

        }

        return self::$instances[$group][$slave];
    }

    private function __construct($group_conf, $slave, $sharding_key)
    {

        $hasher = new ConsistentHash;
        $hasher->addTargets(isset($group_conf[0]) ? $group_conf : [$group_conf]);

        $this->pdo = self::_getMysql($hasher, $sharding_key, $slave);
        $this->slave = $slave;

    }

    private static function _getMysql($hasher, $sharding_key, $slave)
    {
        $target = $hasher->lookupTarget($sharding_key);
        if ($slave && isset($target['slave'])) {
            $target = $target['slave'];

            if (isset($target[0])) $target = Common::array_rand_elements($target, 1);
        }

        $mysql_key = $target['host'] . ':' . $target['port'];
        if (!isset(self::$mysql_map[$mysql_key])) {

            $dsn = sprintf('mysql:host=%s;port=%d;dbname=%s;charset=%s', $target['host'], $target['port'], $target['dbname'], $target['encode']);
            self::$mysql_map[$mysql_key] = new \PDO($dsn, $target['user'], $target['psw']);

        }

        return self::$mysql_map[$mysql_key];
    }

    private function _escapeIdentifiers($id)
    {
        return preg_replace('/[^A-Za-z0-9_]+/', '', $id);
    }

    private function _generateForBinding($data, $group = '')
    {
        $columns = [];
        $values = [];
        $bindings = [];
        foreach ($data as $column => $value) {
            $escaped_column = $this->_escapeIdentifiers($column);
            $columns[] = $escaped_column;
            $binding_name = ':' . $escaped_column . ($group ? "_$group" : '');
            $values[] = $binding_name;
            $bindings[$binding_name] = $value;
        }

        return [$columns, $values, $bindings];
    }

    function get($table, $where)
    {
        $result = $this->find($table, $where);
        return $result ? $result[0] : null;
    }

    function find($table, $where, $limit = null)
    {
        list($where_columns, $where_values, $where_bindings) = $this->_generateForBinding($where);

        $count = count($where_columns);

        $where_array = [];
        for ($i = 0; $i < $count; $i++) {
            $where_array[] = $where_columns[$i] . '=' . $where_values[$i];
        }

        $where_list = implode(' AND ', $where_array);

        $sql = "SELECT * FROM $table WHERE $where_list" . ($limit ? "LIMIT $limit" : '');
        return $this->query($sql, $where_bindings);
    }

    function count($table, $where)
    {
        list($where_columns, $where_values, $where_bindings) = $this->_generateForBinding($where);

        $count = count($where_columns);

        $where_array = [];
        for ($i = 0; $i < $count; $i++) {
            $where_array[] = $where_columns[$i] . '=' . $where_values[$i];
        }

        $where_list = implode(' AND ', $where_array);

        $sql = "SELECT count(*) FROM $table WHERE $where_list";

        $stmt = $this->pdo->prepare($sql);
        $stmt->execute($where_bindings);
        return $stmt->fetchColumn();
    }

    function insert($table, $data)
    {
        list($columns, $values, $bindings) = $this->_generateForBinding($data);

        $column_list = implode(',', $columns);
        $value_list = implode(',', $values);

        $sql = "INSERT INTO " . $this->_escapeIdentifiers($table) . "($column_list) VALUES($value_list)";
        return $this->execute($sql, $bindings);
    }

    function update($table, $where, $updates)
    {
        list($where_columns, $where_values, $where_bindings) = $this->_generateForBinding($where, 'where');

        $count = count($where_columns);

        $where_array = [];
        for ($i = 0; $i < $count; $i++) {
            $where_array[] = $where_columns[$i] . '=' . $where_values[$i];
        }

        $where_list = implode(' AND ', $where_array);

        list($update_columns, $update_values, $update_bindings) = $this->_generateForBinding($update, 'update');

        $count = count($update_columns);

        $update_array = [];
        for ($i = 0; $i < $count; $i++) {
            $update_array[] = $update_columns[$i] . '=' . $update_values[$i];
        }

        $update_list = implode(',', $update_array);

        $sql = "UPDATE " . $this->_escapeIdentifiers($table) . " SET $update_list WHERE $where_list";
        return $this->execute($sql, array_merge($where_bindings, $update_bindings));
    }

    function delete($table, $where)
    {
        list($where_columns, $where_values, $where_bindings) = $this->_generateForBinding($where);

        $count = count($where_columns);

        $where_array = [];
        for ($i = 0; $i < $count; $i++) {
            $where_array[] = $where_columns[$i] . '=' . $where_values[$i];
        }

        $where_list = implode(' AND ', $where_array);

        $sql = "DELETE " . $this->_escapeIdentifiers($table) . " WHERE $where_list";
        return $this->execute($sql, $bindings);
    }

    function lastInsertId()
    {
        return $this->pdo->lastInsertId();
    }

    function execute($sql, $bindings = NULL)
    {
        $stmt = $this->pdo->prepare($sql);
        return $stmt->execute($bindings);
    }

    function query($sql, $bindings = NULL)
    {
        $stmt = $this->pdo->prepare($sql);
        $stmt->execute($bindings);
        return $stmt->fetchAll(\PDO::FETCH_ASSOC);
    }

}
