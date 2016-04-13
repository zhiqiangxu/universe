<?php
include __DIR__ . "/common.php";

$options = get_options();
$remaining_args = get_remaining_args();

$port = $remaining_args[0];

$run_cmd = "sudo tcpdump -i any port $port -v -A";

shell_exec_realtime_output($run_cmd);

