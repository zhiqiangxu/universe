<?php
require(__DIR__ . "/../build/php/ReactHandler.php");

$s=new SocksServer();
$s->listen(8082);
$s->start();
