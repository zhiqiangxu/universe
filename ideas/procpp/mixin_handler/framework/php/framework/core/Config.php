<?php
namespace ReactHandler;

class Config implements \ArrayAccess
{
    private $config_path = [];
    private $config = [];

    function setPath($dir)
    {
        array_unshift($this->config_path, $dir);
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
