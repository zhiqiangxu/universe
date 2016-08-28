<?php

define('WEBPATH', __DIR__);
define('ENV', get_cfg_var('env.name') ? get_cfg_var('env.name') : 'dev');

error_reporting(E_ALL ^ E_NOTICE ^ E_WARNING ^ E_DEPRECATED ^ E_USER_NOTICE);

date_default_timezone_set('Asia/Shanghai');

//load framework
require __DIR__ . "/framework/init.php";


