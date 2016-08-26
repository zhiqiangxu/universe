<?php
namespace Handler\Client;

use Handler\Design\IRedis;

class Redis implements IRedis
{

    private $_connection_info;
    private $_redis;

    const DEFAULT_TIMEOUT = 2.5;
    const RETRY_TIMES = 2;

    function __construct($connection_info)
    {

        $this->_connection_info = $connection_info;

        $this->connect();

    }

    //kv
    function set($key, $data)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $success = $this->_redis->set($key, $data);

                if (!$success) {

                    $this->connect();
                    continue;

                }

                return $success;

            } catch(\RedisException $e){

                $this->connect();

            }

        }

        return false;

    }

    function get($key)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $result = $this->_redis->get($key);

                if ($result === false) {

                    $this->connect();
                    continue;

                }

                return $result;

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function del($key)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->del($key);

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function setNx($key, $data)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $success = $this->_redis->setnx($key, $data);

                if (!$success) {

                    $this->connect();
                    continue;

                }

                return $success;

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function setEx($key, $data, $expire)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $success = $this->_redis->setex($key, $data, $expire);

                if (!$success) {

                    $this->connect();
                    continue;

                }

                return $success;

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function setNxEx($key, $data, $expire)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $success = $this->_redis->set( $key, $data, [ 'nx', 'ex' => $expire ] );

                if (!$success) {

                    $this->connect();
                    continue;

                }

                return $success;

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function incr($key, $by = 1)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->incrBy( $key, $by );

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function decr($key, $by = 1)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->decrBy( $key, $by );

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    //hash
    function hSet($key, $attr, $value)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $result = $this->_redis->hSet( $key, $attr, $value );

                if ($result === false) {

                    $this->connect();
                    continue;

                }

                return $result;

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function hGet($key, $attr)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->hGet( $key, $attr );

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function hGetAll($key)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->hGetAll( $key );

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return false;

    }

    function __call($method, $params)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            return call_user_func_array([$this->_redis, $method], $params);

         } catch(\RedisException $e) {

            $this->connect();

        }

        return false;

    }

    private function connect()
    {

        $this->_redis = new \Redis;

        $timeout = isset($this->_connection_info['timeout']) ? $this->_connection_info['timeout'] : self::DEFAULT_TIMEOUT;

        try {

            if (!empty($this->_connection_info['pconnect'])) {

                $this->_redis->pconnect($this->_connection_info['host'], $this->_connection_info['port'], $timeout);

            } else {

                $this->_redis->connect($this->_connection_info['host'], $this->_connection_info['port'], $timeout);

            }

        } catch(\RedisException $e){
        }

    }

}
