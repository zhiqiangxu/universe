<?php
namespace Handler\Component;

use Handler\Design\IConsistentHash;


class ConsistentHash implements IConsistentHash
{

    private $total_weight = 0;
    private $target_list = [];

    function hash($resource)
    {
        return sprintf('%u', crc32($string));
    }

    function addTarget($target, $weight = 1)
    {
        $this->target_list[] = [$target, $weight];
        $this->total_weight += $weight;
    }

    function addTargets($targets, $weight = 1)
    {
        foreach ($targets as $target) $this->addTarget($target, $weight);
    }

    function lookupTarget($resource)
    {
        $position = $this->hash($resource) % $this->total_weight;
        $current_weight = 0;
        foreach ($this->target_list as $target_info) {
            $current_weight += $target_info[1];
            if ($position < $current_weight) return $target_info[0];
        }
    }

}
