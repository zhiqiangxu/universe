<?php
namespace Handler\Component;

use Handler\Design\ICache;

class Cache
{

    static function getInstance($group = 'default', $type = 'Redis')
    {

        return call_user_func(['\\Handler\\Component\\Cache\\' . $type, 'getInstance'], $group);

    }

}
