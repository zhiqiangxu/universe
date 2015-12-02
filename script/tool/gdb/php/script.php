<?php

function __gdb_php_eval($code)
{
	$gdb_cmd = 'p zend_eval_string(' . json_encode($code) . ', 0, "")';
	return $gdb_cmd;
}
