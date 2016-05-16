<?php
include __DIR__ . "/common.php";


$pids = `pgrep -d',' -f {$argv[1]}`;
$top_cmd = "top -c -p $pids";

shell_exec_realtime_output($top_cmd);
