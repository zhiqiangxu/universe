<?php
include __DIR__ . "/common.php";

$options = get_options(['init', '*-script']);
$remaining_args = get_remaining_args();

$pid = array_shift($remaining_args);
array_unshift($remaining_args, "attach $pid");


$gdb_batch_cmd = implode("\n", $remaining_args) . "\n";

if (!empty($options['init']))
{
	$init_content = '';
	$gdb_inits = explode(',', $options['with']);
	foreach ($gdb_inits as $gdb_init)
	{
		$init_files = glob_files($gdb_init, __DIR__ . DIRECTORY_SEPARATOR . 'gdb');
		if (!$init_files)
		{
			$last_try_file = __DIR__ . DIRECTORY_SEPARATOR . 'gdb' . DIRECTORY_SEPARATOR . $gdb_init . DIRECTORY_SEPARATOR . 'init';
			if (file_exists($last_try_file)) $init_files = [ $last_try_file ];
		}
		foreach ($init_files as $init_file)
		{
			$init_content .= file_get_contents($init_file) . "\n";
		}
	}

	$gdb_batch_cmd = $init_content . $gdb_batch_cmd;
	unset($options['init']);
}

if ($options)
{
	$script_cmd = '';
	$script_options = array_keys($options);
	foreach ($script_options as $script_option)
	{
		list ($language, $_) = explode('-', $script_option);
		myassert(file_exists($script_file = __DIR__ . DIRECTORY_SEPARATOR . 'gdb' . DIRECTORY_SEPARATOR . $language . DIRECTORY_SEPARATOR . 'script.php'), "$language script file not exists");

		require $script_file;
		$call = '__gdb_' . $language . '_eval';
		$script_cmd .= call_user_func($call, $options[$script_option]) . "\n";
	}

	$gdb_batch_cmd .= $script_cmd;
}

$gdb_cmd_file = tempnam('/tmp', '');
file_put_contents($gdb_cmd_file, $gdb_batch_cmd);
$gdb_cmd = "sudo gdb --batch --command=$gdb_cmd_file";

shell_exec_realtime_output($gdb_cmd);

unlink($gdb_cmd_file);
