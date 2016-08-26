<?php
namespace Handler\Component;
use Handler\Design\IConfig;

class Config implements IConfig
{
    private $config_path = [];
    private $config = [];

    function addPath($dir)
    {
        $this->config_path[] = $dir;
    }

    private function load($index)
    {
        foreach ($this->config_path as $path) {
            $filename = $path . '/' . $index . '.php';

            if (is_file($filename)) {

                $retData = include $filename;

                if ($retData) $this->config[$index] = $retData;

            }
        }
    }

    function offsetGet($index)
    {
        if (!isset($this->config[$index])) {
            $this->load($index);
        }

        return isset($this->config[$index]) ? $this->config[$index] : null;
    }

    function offsetSet($index, $newval)
    {
        $this->config[$index] = $newval;
    }

    function offsetUnset($index)
    {
        unset($this->config[$index]);
    }

    function offsetExists($index)
    {
        return isset($this->config[$index]);
    }
}
