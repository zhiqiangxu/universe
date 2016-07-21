<?php
namespace ReactHandler;

use Handler\Http;
use Config;

class Handler
{
    static $php;

    public $request;
    public $response;

    public $config;

    const DEFAULT_FACTORY_KEY = 'master';
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
    private $factory_key;
    private $factory_shard_key;

    private function __construct()
    {
        $this->config = new Config;
        $this->config->setPath(WEBPATH . '/conf');
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
            exit("exit on exception\n");
        }
    }

    function __get($lib_name)
    {
        if (empty($this->$lib_name)) {
            $this->loadModule($lib_name);
        }

        return $this->$lib_name;
    }

    /* 框架层面粒度到key, shard_key, 更细的粒度由模块实现*/
    private function loadModule($module, $key = self::DEFAULT_FACTORY_KEY, $shard_key = null)
    {
        $object_id = $module . '_' . $key . '_' . $shard_key;
        if (empty($this->objects[$object_id])) {
            $this->factory_key = $key;
            $this->factory_shard_key = $shard_key;

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


