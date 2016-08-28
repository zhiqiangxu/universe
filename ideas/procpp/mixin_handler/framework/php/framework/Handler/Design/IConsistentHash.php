<?php
namespace Handler\Design;


interface IConsistentHash
{
    function hash($resource);
    function addTarget($target, $weight);
    function addTargets($targets, $weight);
    function lookupTarget($resource);
}
