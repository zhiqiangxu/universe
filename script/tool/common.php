<?php

/*
 * 以linebuffer模式运行命令并输出
 */
function shell_exec_realtime_output($cmd) {
	echo "executing:\t$cmd\n\n";
	echo str_repeat("#", 20) . "\n";
	system(term_cmd($cmd), $ret);
	echo str_repeat("#", 20) . "\n\n";

	myassert($ret == 0, "fail cmd:\t$cmd\n");
}

function shell_exec_no_output($cmd) {
	$bash_cmd = bash_cmd($cmd);
	exec($bash_cmd, $_, $return);

	myassert($return == 0, "fail compiling:\t$cmd\n" . implode("\n", $_));
}

/*
 * 交互式运行命令
 */
function term_cmd($cmd) {
	return 'script -qec ' . escapeshellarg($cmd) . ' /dev/null';
}

function bash_cmd($cmd) {
	/**
	  -i 读 .bashrc 且 expand_aliases
	  -l 读 .bash_profile
	  /dev/null 防止读取stdin导致TTIN signal
	**/
	return 'bash -lic ' . escapeshellarg($cmd . ' < /dev/null');
}

function trimlr($s) {
	return rtrim(trim($s));
}

function at_exit($callable) {
	static $AT_EXIT_JOBS = [];
	if (!$AT_EXIT_JOBS) {
		$onexit = function() use (&$AT_EXIT_JOBS) {
			foreach($AT_EXIT_JOBS as $callable) {
				call_user_func($callable);
			}
		};
		pcntl_signal(SIGINT, function ($signo) use ($onexit) {
			call_user_func($onexit);
			exit;
		});
		register_shutdown_function($onexit);
	}
	$AT_EXIT_JOBS[] = $callable;
}

function _on_exit() {
	global $AT_EXIT_JOBS;
}

function backtrace($flag = null) {
	static $backtrace = false;
	if ($flag !== null) $backtrace = $flag;

	return $backtrace;
}
/*
 * 报错并退出
 */
function error_exit($message) {
	if (backtrace()) {
		print_r(debug_backtrace());
		echo "\n";
	}
	echo $message . "\n\n";
	exit;
}

function myassert($exp, $message) {
	if (!$exp) error_exit($message);
}

function in_array_glob($needle, $array) {
	foreach ($array as $pattern) {
		if (fnmatch($pattern, $needle)) return true;
	}

	return false;
}
/*
 * 返回所有选项，调用端需要指定所有需要的选项
 */
$OPTIONS = null;
$REMAINING_ARGS = null;
function get_options($value_opts = [], $novalue_opts = []) {
	global $OPTIONS;
	global $REMAINING_ARGS;
	global $argv;

	if ($OPTIONS !== null) return $OPTIONS;

	$OPTIONS = $REMAINING_ARGS = [];

	$args = array_slice($argv, 1);
	for ($i = 0; $i < count($args); $i++) {
		$arg = $args[$i];
		if (mb_substr($arg, 0, 1) == '-') {
			$arg = mb_substr($arg, 1);
			if (in_array_glob($arg, $value_opts)) {
				if (isset($OPTIONS[$arg])) {
					$values = is_array($OPTIONS[$arg]) ? $OPTIONS[$arg] : [$OPTIONS[$arg]];
					$values[] = $args[++$i];
					$OPTIONS[$arg] = $values;
				} else {
					$OPTIONS[$arg] = $args[++$i];
				}
			} else if (in_array_glob($arg, $novalue_opts)) {
				$OPTIONS[$arg] = 1;
			} else {
				error_exit("undefined option:\t$arg");
			}
		} else {
			$REMAINING_ARGS[] = $arg;
		}
	}

	return $OPTIONS;
}

function get_remaining_args() {
	global $REMAINING_ARGS;
	myassert($REMAINING_ARGS !== null, "Please call get_options first!");

	return $REMAINING_ARGS;
}

function glob_files($globs, $additional_path = null, $allow_empty = false) {
	if (is_string($globs)) $globs = [$globs];
	$files = [];
	foreach ($globs as $pattern) {
		$matching_files = glob($pattern);
		if (!$matching_files) {
			if ($additional_path) {
				if (is_string($additional_path)) $additional_path = [$additional_path];
				foreach ($additional_path as $path) {
					if ($matching_files = glob($path . "/" . $pattern)) break;
				}
			}
		}
		if (!$allow_empty) myassert($matching_files, "unmatched glob:\t$pattern");

		$realpath_files = array_map(function($f) { return realpath($f); }, $matching_files);
		$files = array_merge($files, array_flip($realpath_files));
	}

	return array_keys($files);
}

function file_extension($file) {
	if ($lastdotidx = mb_strrpos($file, '.')) {
		return mb_substr($file, $lastdotidx+1);
	}

	return null;
}

function arraylize($thing) {
	return is_array($thing) ? $thing : [$thing];
}

function shell_expand_string($shell_string) {
	$cmd = "echo -n $shell_string";
	return shell_exec($cmd);
}

const C_COMPILER = 'gcc';
const CXX_COMPILER = 'g++';
const COMPILER_MAP = ['c' => C_COMPILER, 'c++' => CXX_COMPILER, 'cpp' => CXX_COMPILER];
function get_compiler($source) {
	$ext = file_extension($source);
	myassert(array_key_exists($ext, COMPILER_MAP), "unrecognized extension for $source!");

	return COMPILER_MAP[$ext];
}

function is_source_file($file) {
	$ext = file_extension($file);
	return array_key_exists($ext, COMPILER_MAP);
}

const  BUILD_PATH = '/tmp/build/';
const  LD_PATH = '/tmp/build/ld/';
if (!file_exists(LD_PATH)) mkdir(LD_PATH, 0777, true);
function get_buildpath($file) {
	myassert($rp_file = realpath($file), "file not exists: $file");

	$path = str_replace([DIRECTORY_SEPARATOR, '.'], '_', $rp_file);
	return BUILD_PATH . $path;
}

function compile($sources, $cflags = '', $ldflags = '', $out = null) {
	if (is_array($cflags)) $cflags = implode(' ', $cflags);
	if (is_array($ldflags)) $ldflags = implode(' ', $ldflags);

	$objects = [];
	$hascpp = false;
	foreach ($sources as $source) {
		$compiler = get_compiler($source);
		if ($compiler == CXX_COMPILER) $hascpp = true;
		$build_path = get_buildpath($source);
		$object = $build_path . '.o';
		$cmd = "$compiler $cflags -Wall -c $source -o $object";
		shell_exec_no_output($cmd);
		$objects[] = $object;
	}

	$ld = $hascpp ? CXX_COMPILER : C_COMPILER;
	if (!$out) $out = tempnam(LD_PATH, '');
	$cmd = "$ld -Wl,--no-as-needed $ldflags " . implode(' ', array_map(function($o) { return escapeshellarg($o); }, $objects)) . " -o " . escapeshellarg($out);
	shell_exec_no_output($cmd);

	return $out;
}

function compile_pre_so($source, $cflags = '', $ldflags = '', $out = null) {
	$compiler = get_compiler($source);
	$build_path = get_buildpath($source);
	$object = $build_path . '.o';
	$cmd = "$compiler -D_GNU_SOURCE -Wall -fPIC -DPIC -c " . escapeshellarg($source) . " -o $object";
	shell_exec_no_output($cmd);
	if (!$out) $out = get_buildpath($source) . ".so";
	$cmd = "ld -shared -o $out $object -ldl";
	shell_exec_no_output($cmd);

	return $out;
}
