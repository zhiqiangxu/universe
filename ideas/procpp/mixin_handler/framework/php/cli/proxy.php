<?php
require __DIR__ . "/../init.php";


$s = new ClientServer;
$protocol = new Proxy($s, [['127.0.0.1', 6379]]);
$s->listen(8082, $protocol);
$s->event_loop();


