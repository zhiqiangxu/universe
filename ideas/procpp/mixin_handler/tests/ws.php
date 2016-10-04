<?php
require(__DIR__ . "/../build/php/ReactHandler.php");
class PhpCallback extends WebSocketCallback {
    function run($m, $ws)
    {
        if ($m->opcode == WebSocket::OPCODE_TEXT_FRAME) $ws->send($m->client, $m->payload);
    }
}

$callback = new PhpCallback();
$callback->thisown = 0;

$s=new WebSocketServer();
$s->on('message', $callback);
$s->listen(8082);
$s->event_loop();
