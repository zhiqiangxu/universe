<?php

class Test
{
    final protected $property;
    function test($w)
    {
    }
}

class Hi implements Test
{
    protected $property;

    function test($w)
    {
        echo "$w\n";
    }
}

$h = new Hi;
$h->test("hahah");
