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

    // Bool TRUE if the command is successful.
    // else null is returned.
    function set($key, $data, $options = null)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $success = is_null($options) ? $this->_redis->set($key, $data) : $this->_redis->set($key, $data, $options);

                if (!$success) {

                    $this->connect();
                    continue;

                }

                return true;

            } catch(\RedisException $e){

                $this->connect();

            }

        }

        return null;

    }


    // If key didn't exist, FALSE is returned. Otherwise, the value related to this key is returned.
    // on RedisException, null is returned.
    function get($key)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->get($key);

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    // Long Number of keys deleted.
    // on RedisException null is returned.
    function del($key)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->del($key);

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    // TRUE in case of success
    // else null is returned.
    function setNx($key, $data)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $success = $this->_redis->setnx($key, $data);

                if (!$success) {

                    $this->connect();
                    continue;

                }

                return true;

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    // TRUE if the command is successful.
    // else null is returned.
    function setEx($key, $data, $expire)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $success = $this->_redis->setex($key, $expire, $data);

                if (!$success) {

                    $this->connect();
                    continue;

                }

                return true;

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    // TRUE if the command is successful.
    // else null is returned.
    function setNxEx($key, $data, $expire)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                $success = $this->_redis->set( $key, $data, [ 'nx', 'ex' => $expire ] );

                if (!$success) {

                    $this->connect();
                    continue;

                }

                return true;

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    // the new value
    // on RedisException null returned
    function incr($key, $by = 1)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->incrBy( $key, $by );

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    // the new value
    // on RedisException null returned
    function decr($key, $by = 1)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->decrBy( $key, $by );

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    // LONG 1 if value didn't exist and was added successfully, 0 if the value was already present and was replaced
    // else null returned
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

        return null;

    }

    // Gets a value from the hash stored at key. If the hash table doesn't exist, or the key doesn't exist, FALSE is returned
    // on RedisException null returned
    function hGet($key, $attr)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->hGet( $key, $attr );

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    // Returns the whole hash, as an array of strings indexed by strings.
    // on RedisException null returned
    function hGetAll($key)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try {

                return $this->_redis->hGetAll( $key );

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

    }

    function __call($method, $params)
    {

        for ($i = 0; $i < self::RETRY_TIMES; $i++) {

            try{

                return call_user_func_array([$this->_redis, $method], $params);

            } catch(\RedisException $e) {

                $this->connect();

            }

        }

        return null;

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
