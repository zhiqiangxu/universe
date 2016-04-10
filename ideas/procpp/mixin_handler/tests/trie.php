<?php
require(__DIR__ . "/../build/php/ReactHandler.php");

$keywords = ['中国', '国人', 'ab', 'bc', 'cd'];
for ($i = 0; $i < 1000; $i++) {
    $keywords[] = uniqid();
}

$s = str_repeat('中国人abc isn cd', 100);

for ($i = 0; $i < 100; $i++) {
	$time = microtime(true);

	$trie = new Trie($keywords);
	var_dump($trie->search($s));

	echo "tooks " . (microtime(true) - $time) . "\r\n";
}
