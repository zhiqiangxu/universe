<?php
namespace Handler\Design;

interface IDatabase
{
    //成功返回结果集
    //失败返回false
    function query($sql, $bindings = []);

    //成功返回true
    //失败返回false
    function execute($sql, $bindings = []);

    //helpers

    function get($tables, $parameters);
    function find($tables, $parameters);
    function count($tables, $parameters);
    function insert($tables, array $data, $return_id = false);
    function lastInsertId($id_name = null);
    function update($tables, $where_parameters, array $update_data);
    function delete($tables, $where_parameters);
}
