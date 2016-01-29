<?php
include __DIR__ . "/common.php";


get_options();


$remaining_args = get_remaining_args();

list($str1, $str2, $dir) = $remaining_args;


$run_cmd = "find $dir -type f -exec sed -i " . escapeshellarg("s/$str1/$str2/g") . " {} \\;";


shell_exec_realtime_output($run_cmd);
