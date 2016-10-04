<?php
require __DIR__ . "/../init.php";


$s = new ClientServer();
$proto = new Remote($s, [['127.0.0.1', 8084]], 8085);
$s->listen(8083, $proto);
$s->event_loop();

