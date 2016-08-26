<?php
namespace Handler\Client;
use Handler\Design\IDatabase;

class MySQL implements IDatabase
{

    const RETRY_TIMES = 3;

    private $_connection_info;
    private $_pdo;

    function __construct($connection_info)
    {

        $this->_connection_info = $connection_info;

        $this->connect();

    }

    function query($sql, $bindings = [], array $fetch_parameters = null)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            $stmt = $this->_pdo->prepare($sql);

            if (!$stmt) {

                $this->connect();
                continue;

            }

            $success = $bindings ? $stmt->execute($bindings) : $stmt->execute();

            if (!$success) {

                $this->connect();
                continue;

            }

            return $fetch_parameters ? call_user_func_array([$stmt, 'fetchAll'], $fetch_parameters) : $stmt->fetchAll(\PDO::FETCH_ASSOC);

        }

        //失败
        return false;

    }

    function execute($sql, $bindings = [])
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            $stmt = $this->_pdo->prepare($sql);

            if (!$stmt) {

                $this->connect();
                continue;

            }

            $success = $bindings ? $stmt->execute($bindings) : $stmt->execute();

            if (!$success) {

                $this->connect();
                continue;

            }

            return true;

        }

        //失败
        return false;

    }

    private function connect()
    {
        $dsn = sprintf(
                'mysql:host=%s;port=%d;dbname=%s;charset=%s',
                $this->_connection_info['host'],
                $this->_connection_info['port'],
                $this->_connection_info['dbname'],
                $this->_connection_info['encode']
        );

        $this->_pdo = new \PDO($dsn, $this->_connection_info['user'], $this->_connection_info['psw']);
    }

    function get($tables, $parameters)
    {

        $parameters['limit'] = 1;
        $result = $this->find($table, $parameters);
        return $result ? $result[0] : $result;

    }

    function find($tables, $parameters)
    {

        if (!is_array($tables)) $tables = [$tables];


        list($where, $where_bindings) = $this->getWhereAndBindings($parameters);

        $order = empty($parameters['order']) ? '' : "ORDER BY {$parameters['order']}";
        $limit = empty($parameters['limit']) ? '' : "LIMIT {$parameters['limit']}";


        $sqls = [];
        foreach ($tables as $table) {
            $sql[] = "SELECT * FROM $table WHERE $where $order $limit";
        }

        $sql = implode('UNION ALL', $sqls);

        return $this->query($sql, $where_bindings);

    }

    function count($tables, $parameters)
    {

        if (!is_array($tables)) $tables = [$tables];


        list($where, $where_bindings) = $this->getWhereAndBindings($parameters);

        $sqls = [];
        foreach ($tables as $table) {
            $sql[] = "SELECT count(*) FROM $table WHERE $where";
        }

        $sql = implode('UNION ALL', $sqls);

        $result = $this->query($sql, $where_bindings, [\PDO::FETCH_COLUMN, 0]);

        if (!$result) return 0;

        $count = 0;
        foreach ($result as $table_count) $count += $table_count;
        return $count;

    }

    //$return_ids只应在插入多个表时为true
    //单个表的情况，建议通过调用lastInsertId()获取
    function insert($tables, array $data, $return_id = false, $id_name = null)
    {

        if (!is_array($tables)) $tables = [$tables];

        list($binding_data, $inline_data) = [
            empty($data['binding_data']) ? null : $data['binding_data'],
            empty($data['inline_data']) ? null : $data['inline_data']
        ];
        list($columns, $values, $bindings) = $this->generateForData($binding_data, $inline_data);

        $column_list = implode(',', $columns);
        $value_list = implode(',', $values);

        $ids = [];
        foreach ($tables as $table) {

            $sql = "INSERT INTO $table ($column_list) VALUES($value_list)";
            $success = $this->execute($sql, $bindings);
            //其中一次失败，则认为整体失败
            if (!$success) false;

            if (!$return_ids) continue;

            //id序同table序
            $ids[] = $this->lastInsertId($id_name);

        }

        return $return_ids ? $ids : true;

    }

    function lastInsertId($id_name = null)
    {

        return $this->_pdo->lastInsertId($id_name);

    }

    function update($tables, $where_parameters, array $update_data)
    {

        if (!is_array($tables)) $tables = [$tables];

        list($where, $where_bindings) = $this->getWhereAndBindings($where_parameters);

        list($binding_data, $inline_data) = [
            empty($update_data['binding_data']) ? null : $update_data['binding_data'],
            empty($update_data['inline_data']) ? null : $update_data['inline_data']
        ];
        list($columns, $values, $update_bindings) = $this->generateForData($binding_data, $inline_data, 'update');

        $total_bindings = array_merge($where_bindings, $update_bindings);

        $set = [];
        for ($i = 0; $i < count($columns); $i++) {
            $set[] = "{$columns[$i]}={$values[$i]}";
        }
        $set = implode(',', $set);

        foreach ($tables as $table) {

            $sql[] = "UPDATE $table SET $set WHERE $where";
            $success = $this->execute($sql, $total_bindings);
            //其中一次失败，则认为整体失败
            if (!$success) false;

        }

        return true;

    }

    function delete($tables, $where_parameters)
    {

        if (!is_array($tables)) $tables = [$tables];


        list($where, $where_bindings) = $this->getWhereAndBindings($where_parameters);

        foreach ($tables as $table) {

            $sql = "DELETE FROM $table WHERE $where";
            $success = $this->execute($sql, $where_bindings);
            //其中一次失败，则认为整体失败
            if (!$success) false;

        }

        return true;

    }

    private function getWhereAndBindings($where_parameters)
    {

        if (is_string($where_parameters)) return [$where_parameters, []];

        $where = $where_parameters[0];

        $where_bindings = [];
        if (!empty($where_parameters['bindings'])) {
            foreach ($parameters['bindings'] as $name => $value) {
                $where_bindings[":$name"] = $value;
            }
        }

        return [$where, $where_bindings];

    }

    private function generateForData($binding_data, $inline_data, $group = '')
    {

        $columns = [];
        $values = [];
        $bindings = [];
        foreach ($binding_data as $column => $value) {
            $escaped_column = $this->escapeIdentifiers($column);
            $columns[] = $escaped_column;
            $binding_name = ':' . $escaped_column . ($group ? "_$group" : '');
            $values[] = $binding_name;
            $bindings[$binding_name] = $value;
        }
        if ($inline_data) {
            foreach ($inline_data as $column => $value) {
                $columns[] = $column;
                $values[] = $value;
            }
        }

        return [$columns, $values, $bindings];

    }

    private function escapeIdentifiers($id)
    {
        return preg_replace('/[^A-Za-z0-9_]+/', '', $id);
    }

}
