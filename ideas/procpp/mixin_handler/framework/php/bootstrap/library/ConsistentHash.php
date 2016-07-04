<?php


interface IConsistentHash
{
    public function hash($resource);
    public function addTarget($target, $weight);
    public function addTargets($targets, $weight);
    public function lookupTarget($resource);
}

class ConsistentHash implements IConsistentHash
{

    private $total_weight = 0;
    private $target_list = [];

    public function hash($resource)
    {
        return sprintf('%u', crc32($string));
    }

    public function addTarget($target, $weight = 1)
    {
        $this->target_list[] = [$target, $weight];
        $this->total_weight += $weight;
    }

    public function addTargets($targets, $weight = 1)
    {
        foreach ($targets as $target) $this->addTarget($target, $weight);
    }

    public function lookupTarget($resource)
    {
        $position = $this->hash($resource) % $this->total_weight;
        $current_weight = 0;
        foreach ($this->target_list as $target_info) {
            $current_weight += $target_info[1];
            if ($position < $current_weight) return $target_info[0];
        }
    }
}
