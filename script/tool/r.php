<?php
include __DIR__ . "/common.php";

$options = get_options(['rflags', 'cflags', 'cppflags', 'ldflags', 'suffix', 'args'], ['keep', 'backtrace', 'sudo', 'valgrind']);
backtrace(isset($options['backtrace']) ? true : false);

$remaining_args = get_remaining_args();
$source_files = glob_files($remaining_args);

$suffix = array_value($options, 'suffix', '');
$cflags = [];
if (isset($options['cflags'])) $cflags[C_COMPILER] = arraylize($options['cflags']);
if (isset($options['cppflags'])) $cflags[CXX_COMPILER] = arraylize($options['cppflags']);

$ldflags = isset($options['ldflags']) ? arraylize($options['ldflags']) : [];
$alternate_compiler = [];

if (isset($options['rflags'])) load_rflags($options['rflags'], $cflags, $ldflags, $alternate_compiler, $suffix);

foreach ($source_files as $source_file)
{
	$dir = dirname($source_file);

	if (file_exists($rflags_file = $dir . DIRECTORY_SEPARATOR . 'rflags.php'))
	{
		load_rflags($rflags_file, $cflags, $ldflags, $alternate_compiler, $suffix);
	}
}
if ($cflags) {
	// cppflags自动包含cflags
	$cflags[CXX_COMPILER] = array_merge(array_value($cflags, CXX_COMPILER, []), array_value($cflags, C_COMPILER, []));
}
$compile_opts = ['cflags' => $cflags, 'ldflags' => $ldflags, 'alternate_compiler' => $alternate_compiler, 'suffix' => $suffix];
$executable = compile($source_files, $compile_opts);
if ($alternate_compiler) {
	// 如果有alternate_compiler，只编译不执行
	echo "compiled to $executable\n";
	exit;
}

if (!isset($options['keep'])) {
	at_exit(function() use ($executable) {
		echo "unlink $executable\n";
		unlink($executable);
	});
}


$run_cmd = (isset($options['sudo']) ? 'sudo ' : '') . "$executable" . (isset($options['args']) ? ' ' . $options['args'] : '');
if (isset($options['valgrind'])) $run_cmd = "valgrind $run_cmd";
shell_exec_realtime_output($run_cmd);

