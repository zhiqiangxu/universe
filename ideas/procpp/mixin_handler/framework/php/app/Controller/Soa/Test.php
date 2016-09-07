<?php

namespace App\Controller\Soa;
use Handler\MVC\Soa\Controller;

class Test extends Controller
{
    function test()
    {

        $this->outputJson([1,2,3]);

    }
}
