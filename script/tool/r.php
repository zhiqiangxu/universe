<?php
include __DIR__ . "/common.php";

$options = get_options(['cflags', 'ldflags', 'args'], ['keep', 'backtrace', 'sudo']);
backtrace(isset($options['backtrace']) ? true : false);

$remaining_args = get_remaining_args();
$source_files = glob_files($remaining_args);
$executable = compile($source_files, isset($options['cflags']) ? $options['cflags'] : '', isset($options['ldflags']) ? $options['ldflags'] : '');

if (!isset($options['keep'])) {
	at_exit(function() use ($executable) {
		echo "unlink $executable\n";
		unlink($executable);
	});
}


$run_cmd = (isset($options['sudo']) ? 'sudo ' : '') . "$executable" . (isset($options['args']) ? ' ' . $options['args'] : '');
shell_exec_realtime_output($run_cmd);

