<?php

define('WEBPATH', dirname(__DIR__));
define('ENV', get_cfg_var('env.name') ? get_cfg_var('env.name') : 'dev');

date_default_timezone_set('Asia/Shanghai');

error_reporting(E_ALL ^ E_NOTICE ^ E_WARNING ^ E_DEPRECATED ^ E_USER_NOTICE);

spl_autoload_register(function($class) {

    static $module_path = ['Controller' => 'controllers', 'Model' => 'models', '_' => 'library'];

    $parts = explode('\\', $class);

    $module = array_shift($parts);

    $dir_name = isset($module_path[$module]) ? $module_path[$module] : $module_path['_'];

    require (WEBPATH . '/' . $dir_name . '/' . implode('/', $parts) . '.php');

});

// load mvc libs
require __DIR__ . "/controllers/Base.php";
require __DIR__ . "/models/Base.php";
require __DIR__ . "/views/Base.php";

// load libs
require __DIR__ . "/library/Handler.php";
require __DIR__ . "/library/Handler/Http.php";
require __DIR__ . "/library/ReactHandler.php";
require __DIR__ . "/library/ConsistentHash.php";
require __DIR__ . "/library/Common.php";
require __DIR__ . "/library/Client/MySQL.php";
require __DIR__ . "/library/Client/Redis.php";



function load_env_conf($type)
{
    return require WEBPATH . '/conf/' . ENV . '/' . $type . '.php';
}

global $php;
$php = Handler::getInstance();
