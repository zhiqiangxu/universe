<?php

const BASE30 = 'ABCDEFGHJKLMNPQRSTUVWY23456789';

function base30_encoding($num, $padding = 0/*最短长度*/)
{
    $str = "";

	do {
        $str .= BASE30[ $num%30 ];
		$num = intval($num/30);
	} while ($num > 0);

	if (strlen($str) < $padding) $str .= str_repeat(BASE30[0], $padding - strlen($str));
    return $str;
}

function generate_serials($start_id, $num)
{
	$start = base30_encoding($start_id, 5);

	$base30 = str_split(BASE30);

	$end = [];
	while (count($end) < $num) {
		$keys = array_rand($base30, 5);
		$values = array_map(function($key) use ($base30) { return $base30[$key]; }, $keys);
		$str = implode('', $values);
		$end[$str] = 1;
	}

	$result = [];
	foreach ($end as $str => $_) {
		$result[] = serial_split($start . $str);
	}

	return $result;
}

function serial_split($serial, $split_length = 4)
{
	$serial_length = strlen($serial);
	$result = '';
	$idx = 0;
	while ($idx < $serial_length) {
		$next_idx = min($serial_length, $idx + $split_length);
		$result .= substr($serial, $idx, $next_idx - $idx) . '-';
		$idx = $next_idx;
	}

	return rtrim($result, '-');
}

