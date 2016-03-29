<?php
require("build/ReactHandler.php");
$s=new HttpClientServer();

$s->on('request', function(){
});
$s->listen(8082);
$s->start();
