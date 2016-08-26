<?php
namespace Handler\Design;


interface IModel
{

    function find($parameters);
    function get($parameters);
    function count($parameters);
    function delete($parameters);
    function insert($data, $parameters = []);
    function update($parameters, $update_data);

    //分库分表相关
    function tableNames($table_id = null);
    function getShard($shard_id);
    function getAllShards();

}
