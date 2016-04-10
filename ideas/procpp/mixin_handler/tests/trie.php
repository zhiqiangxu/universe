<?php
require(__DIR__ . "/../build/php/ReactHandler.php");

$keywords = ['中国', '国人', 'ab', 'bc', 'cd'];
for ($i = 0; $i < 1000; $i++) {
    $keywords[] = uniqid();
}

$trie = new Trie($keywords);
$s = str_repeat('中国人abc isn cd', 100);

var_dump($trie->search($s));
