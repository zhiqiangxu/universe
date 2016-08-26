<?php

define('LIBPATH', __DIR__);

//autoload for plugins
require __DIR__ . "/vendor/autoload.php";
//autoload for framework
require __DIR__ . "/Handler/Design/ILoader.php";
require __DIR__ . "/Handler/Loader.php";


//load handler
require __DIR__ . "/Handler/Design/IHandler.php";
require __DIR__ . "/Handler.php";
//load extension
require __DIR__ . "/ext/ReactHandler.php";

spl_autoload_register('\\Handler\\Loader::autoload');



global $php;
$php = Handler::getInstance();
