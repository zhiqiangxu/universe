<?php
include __DIR__ . "/../common.php";
$options = get_options(['cflags', 'ldflags']);
$remaining_args = get_remaining_args();

$so_files = [];
$source_files = [];
$cmd_flag = false;
$cmd_parts = [];
foreach ($remaining_args as $arg) {
	$files = glob_files($arg, __DIR__ . '/so', true);
	if ((!$files) || (count($files) == 1 && !is_source_file($files[0]))) $cmd_flag = true;
	if ($cmd_flag) {
		$cmd_parts[] = $arg;
	} else {
		$source_files = array_merge($source_files, array_flip($files));
	}
}
$cflags = isset($options['cflags']) ? $options['cflags'] : '';
$ldflags = isset($options['ldflags']) ? $options['ldflags'] : '';
$so_list = [];
foreach (array_keys($source_files) as $source) {
	$so = compile_pre_so($source, $cflags, $ldflags);
	$so_list[] = $so;
}

$orig_cmd = implode(' ', array_map(function($cmd_part) { return escapeshellarg($cmd_part); }, $cmd_parts));
$cmd = "LD_PRELOAD=" . escapeshellarg(implode(' ', $so_list)) . " $orig_cmd";
shell_exec_realtime_output($cmd);
