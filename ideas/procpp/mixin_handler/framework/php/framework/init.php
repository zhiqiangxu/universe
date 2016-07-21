<?php

define('LIBPATH', __DIR__);

require __DIR__ . "/vendor/autoload.php";
require __DIR__ . "/ext/ReactHandler.php";


require __DIR__ . "/core/Loader.php";
ReactHandler\Loader::addNameSpace('ReactHandler', __DIR__ . '/core');
spl_autoload_register('\\ReactHandler\\Loader::autoload');


global $php;
$php = ReactHandler\Handler::getInstance();
