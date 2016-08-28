<?php

use Handler\Loader;
use Handler\Protocol\Http;
use Handler\Component\Logger;
use Handler\Component\Config;
use Handler\Design\IHandler;

class Handler implements IHandler
{
    static $php;

    public $request = null;
    public $response = null;

    public $config;
    public $logger;

    const DEFAULT_FACTORY_GROUP = 'master';
/*
    const MODULES = [
        'redis' => 1,
        'mongo' => 1,
        'mysql' => 1,
        'tpl'   => 1,
        'cache' => 1,
        'curl'  => 1,
    ];
*/
    private $factory_group;
    private $factory_shard_id;

    private function __construct()
    {

        if ( ! (defined('WEBPATH') && defined('ENV')) ) exit('Please define WEBPATH and ENV!');

        Loader::addNameSpace('Handler', __DIR__ . '/Handler');
        Loader::addNameSpace('App', WEBPATH . '/app');

        $this->config = new Config;
        $this->config->addPath(WEBPATH . '/conf/' . ENV);
        $this->config->addPath(WEBPATH . '/conf');

        $this->logger = Logger::getInstance();

    }

    static function getInstance()
    {
        if (!self::$php) self::$php = new Handler();

        return self::$php;
    }

    function dispatchHttp($request, $response)
    {
        try {
            Http::handle($request, $response);
        } catch (Exception $e) {
            var_dump($e);
            exit("exit on exception\n");
        }
    }

    function __get($lib_name)
    {
        if (empty($this->$lib_name)) {
            $this->$lib_name = $this->loadModule($lib_name);
        }

        return $this->$lib_name;
    }

    /* 框架层面粒度到group, shard_id, 更细的粒度由模块实现*/
    private function loadModule($module, $group = self::DEFAULT_FACTORY_GROUP, $shard_id = null)
    {
        $object_id = $module . '_' . $group . '_' . $shard_id;
        if (empty($this->objects[$object_id])) {
            $this->factory_group = $group;
            $this->factory_shard_id = $shard_id;

            $user_factory_file = WEBPATH . '/factory/' . $module . '.php';
            if (is_file($user_factory_file)) $object = require $user_factory_file;
            else {
                $system_factory_file = LIBPATH . '/factory/' . $module . '.php';
                if (!is_file($system_factory_file)) throw new Exception("module [$module] not found.");

                $object = require $system_factory_file;
            }

            $this->objects[$object_id] = $object;
        }

        return $this->objects[$object_id];
    }

    function __call($func, $param)
    {
        array_unshift($param, $func);
        return call_user_func_array([$this, 'loadModule'], $param);
    }

    function factory($class)
    {
        if (class_exists($class)) {
            $args = func_get_args();
            return call_user_func_array([new ReflectionClass($class), 'newInstance'], array_slice($args, 1));
        }

        return false;
    }
}


