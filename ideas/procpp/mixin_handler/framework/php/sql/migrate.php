<?php

require __DIR__ . '/../init.php';

if (count($argv) == 1) $mode = 'migrate';
else {
    if ( !in_array($argv[1], ["migrate","check_db"]) ){
        echo "error: migrate argv not in enum list!";
        exit;
    }
    $mode = $argv[1];
}

$total_db_conf = load_env_conf('mysql');

if ($mode == "migrate"){
    foreach ($total_db_conf as $conf => $value){
        echo"migrate $conf\n";
        $mysql_confs = get_mysql_config($value);
        migrate_all($mysql_confs, $conf);
    }
} elseif ($mode == "check-db"){
    $real_db_strings = array();
    $mysql_diff_confs =array();
    foreach ($total_db_conf as $conf => $value){
        $mysql_confs = get_mysql_config($value);
        $compare_confs = prepare_check_db($mysql_confs, $conf);
        foreach ($compare_confs as $compare_conf){
            $origin = $compare_conf['origin'];
            $real_db_string = $origin['host'].$origin['user'].$origin['psw'].$origin['dbname'];
            if (! in_array($real_db_string, $real_db_strings)){
                array_push($real_db_strings,$real_db_string);
                array_push($mysql_diff_confs, $compare_conf);
            }
        }
    }
    foreach ($mysql_diff_confs as $mysql_diff_conf){
        $origin = $mysql_diff_conf['origin'];
        $test = $mysql_diff_conf['test'];
        $command = 'mysqldiff --host=%s --user=%s --password=%s %s %s';
        $command = sprintf($command, $origin['host'], $origin['user'], $origin['psw'], $origin['dbname'], $test['dbname']);
        try{
            echo system($command);
            drop_test_db($test);
        } catch (Exception $e) {
            echo $e->getMessage()."\n".json_encode($test)."\n";
        }
    }
}
function check_version_list($version_list){
    sort($version_list);
    $current_version = 0;
    foreach ($version_list as $version){
        if ($version != $current_version + 1){
            return false;
        }
        $current_version++;
    }
    return true;
}

function migrate_all($mysql_confs, $conf){
    $schema_directory = __DIR__ . '/schema/'.$conf."/";
    if(file_exists($schema_directory)){
        $dir = opendir($schema_directory);
        while ($file = readdir($dir)){
            if (preg_match("/\w/", $file)){
                try {
                    echo "----in $file----\n";
                    migrate($mysql_confs, $file, $schema_directory);
                } catch (Exception $e) {
                    echo $e->getMessage();
                }
            }
        }
    }
}

function migrate($mysql_configs, $migrate_name, $schema_directory){
    $update_sqls = array();
    $version_list = array();
    $update_directory = $schema_directory.$migrate_name."/";
    $update_dir = opendir($update_directory);
    $split_factor  = array();
    if (preg_match("/^_/", $migrate_name)){
        $split_file = $update_directory.'split';
        if (!file_exists($split_file)){
            throw new Exception("$migrate_name split file not exist");
        }
        $split_string = file_get_contents($split_file);
        $split_factor = explode(' ', $split_string);
    }

    while($update_file = readdir($update_dir)){
        if (preg_match("/(.*?)_.*\.sql/", $update_file, $match)){
            $version = $match[1];
            array_push($version_list, $version);
            $update_sqls[$version] = file_get_contents($update_directory.$update_file);
        }
    }
    if ( !check_version_list($version_list)){
        throw new Exception("update sql file version error not consistent $migrate_name");

    }
    foreach ($mysql_configs as $mysql_config){
        try {
            $connection = prepare_migrate($mysql_config);
            $current_version = get_version($migrate_name, $connection);
            $update_version = $current_version + 1;
            while(array_key_exists($update_version, $update_sqls)){
                echo "======update to ". $update_version."\n";
                foreach (explode(';', $update_sqls[$update_version]) as $sql){
                    $sqls = array();
                    if (preg_match('/^(\s)*$/', $sql)){
                        continue;
                    }
                    if (count($split_factor)){
                        foreach ($split_factor as $split){
                            $split = ltrim($split);
                            $split = rtrim($split);
                            $actual_sql = $sql;

                            $real_sql = preg_replace('/%s/', $split, $actual_sql);

                            array_push($sqls, $real_sql);
                        }
                    }else{
                        $sqls = array($sql);
                    }
                    foreach ($sqls as $excute_sql){
                       if (! mysql_query($excute_sql, $connection)){
                           throw new Exception("fatal error when migrate $migrate_name trying to update version $version excute\n$excute_sql\n");
                       }
                    }
                }
                update_version($connection, $migrate_name, $update_version);
                $update_version++;
            }
            mysql_close($connection);
        } catch (Exception $e) {
            echo $e->getMessage()."\n".json_encode($mysql_config)."\n";
        }
        
    }
}
function update_version($connection, $migrate_name, $version){
    $update_sql = "UPDATE MIGRATE_HISTORY set `version` = %d WHERE `schema` = '%s'";
    $update_sql = sprintf($update_sql, mysql_real_escape_string($version), mysql_real_escape_string($migrate_name));
    if (!mysql_query($update_sql, $connection)){
        throw new Exception("fatal error when migrate $migrate_name trying to update migrate version $version  error");
    }
}
function get_version($migrate_name, $connection){
    $version_sql = "SELECT version from MIGRATE_HISTORY WHERE `schema` = '%s'";
    $version_sql = sprintf($version_sql, mysql_real_escape_string($migrate_name));
    $create_version_sql = "INSERT INTO MIGRATE_HISTORY (`schema`, `version`) VALUES('%s', 0)";
    $create_version_sql = sprintf($create_version_sql, mysql_real_escape_string($migrate_name));
    $result = mysql_query($version_sql, $connection);

    if ($result){
        $row = mysql_fetch_row($result);
        if ($row){
            return $row[0];
        }else{
            if (mysql_query($create_version_sql, $connection)){
                return 0;
            }
        }
    }    
    throw new Exception("try to get version failed $migrate_name");
}
function prepare_migrate($mysql_config){
    $check_migrate_sql = "SELECT 1 FROM MIGRATE_HISTORY";
    $create_migrate_sql = "CREATE TABLE `MIGRATE_HISTORY` (
                            `schema` varchar(255) NOT NULL PRIMARY KEY,
                            `version` int(11) NOT NULL
                           ) ENGINE=InnoDB DEFAULT CHARSET=utf8";
    $mysql_handler = mysql_connect($mysql_config['host'] . ':' . $mysql_config['port'], $mysql_config['user'], $mysql_config['psw']);
    if (! $mysql_handler){
        throw new Exception("connect error");
    }
    mysql_query("set names " .$mysql_config['encode'], $mysql_handler );
    if (! mysql_select_db($mysql_config['dbname'], $mysql_handler)){
        throw new Exception("database select error");
    }
    
    if (! mysql_query($check_migrate_sql, $mysql_handler)){
        if(! mysql_query($create_migrate_sql, $mysql_handler)){
            throw new Exception("create migrate version table error");
        }
    }
    return $mysql_handler;
}
function prepare_check_db($mysql_configs, $conf){
    $compare_mysql_configs = array();
    foreach ($mysql_configs as $mysql_config){
        $test_config = $mysql_config;
        $test_config['dbname'] = 'test_db_for_migrate_'.$test_config['dbname'];
        try{
            create_test_db($test_config);
            migrate_all(array($test_config), $conf);
            array_push( $compare_mysql_configs, array('origin' => $mysql_config, 'test' => $test_config));
        } catch (Exception $e) {
            echo "create test db ".$e->getMessage()."\n".json_encode($test_config)."\n";
        }
    }
    return $compare_mysql_configs;
}
function create_test_db($test_config){
    $create_database_sql = "CREATE DATABASE %s";
    $test_mysql_handler = mysql_connect($test_config['host'] . ':' . $test_config['port'], $test_config['user'], $test_config['psw']);
    $create_database_sql = sprintf($create_database_sql, $test_config['dbname']);
    if (! $test_mysql_handler){
        throw new Exception("connect error");
    }
    if (! mysql_select_db($test_config['dbname'], $test_mysql_handler)){
        if(! mysql_query($create_database_sql, $test_mysql_handler)){
            throw new Exception("create database error");
        }
    }
}
function drop_test_db($test_config){
return;
    $sql = "DROP DATABASE %s";
    $test_mysql_handler = mysql_connect($test_config['host'] . ':' . $test_config['port'], $test_config['user'], $test_config['psw']);
    $sql = sprintf($sql, $test_config['dbname']);
    if (! $test_mysql_handler){
        throw new Exception("connect error");
    }
    if(! mysql_query($sql, $test_mysql_handler)){
        throw new Exception("drop database error");
    }
}
function get_mysql_config($array){

    //兼容分库
    return isset($array[0]) ? $array : [ $array ];
}

?>
