<?php
include __DIR__ . "/common.php";

$options = get_options(['cflags', 'ldflags', 'args'], ['keep', 'backtrace', 'sudo']);
backtrace(isset($options['backtrace']) ? true : false);

$remaining_args = get_remaining_args();
$source_files = glob_files($remaining_args);

$rflags_once = [];
$cflags = isset($options['cflags']) ? arraylize($options['cflags']) : [];
$ldflags = isset($options['ldflags']) ? arraylize($options['ldflags']) : [];
foreach ($source_files as $source_file)
{
	$dir = dirname($source_file);
	if (isset($rflags_once[$dir])) continue;
	$rflags_once[$dir] = 1;

	if (file_exists($rflags_file = $dir . DIRECTORY_SEPARATOR . 'rflags.php'))
	{
		$rflags = require $rflags_file;
		if (isset($rflags['cflags'])) $cflags[] = shell_expand_string($rflags['cflags']);
		if (isset($rflags['ldflags'])) $ldflags[] = shell_expand_string($rflags['ldflags']);
	}
}
$executable = compile($source_files, $cflags, $ldflags);

if (!isset($options['keep'])) {
	at_exit(function() use ($executable) {
		echo "unlink $executable\n";
		unlink($executable);
	});
}


$run_cmd = (isset($options['sudo']) ? 'sudo ' : '') . "$executable" . (isset($options['args']) ? ' ' . $options['args'] : '');
shell_exec_realtime_output($run_cmd);

