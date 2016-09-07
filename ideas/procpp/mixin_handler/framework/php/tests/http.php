<?php
$start = microtime(true);

file_get_contents('http://test.com');
echo "took " . (microtime(true) - $start) . " seconds\n";

$start = microtime(true);
file_get_contents('http://test.com:8082');
echo "took " . (microtime(true) - $start) . " seconds\n";
