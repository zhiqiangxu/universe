<?php
function doGenerateHighlight(Array $keywords, $content, $caseInsensitive = false)
{
	$exp = '/'
	. implode('|', array_map('preg_quote', $keywords))
	. ($caseInsensitive ? '/i' : '/');
	preg_match_all($exp, $content, $matches);

	if ($matches[0]) {
		return array_unique($matches[0]);
	}

	return [];
}

$keywords = ['ab', 'bc', 'cd'];
for ($i = 0; $i < 1000; $i++) {
	$keywords[] = "ef";
}
$time = microtime(true);
var_dump(doGenerateHighlight($keywords, str_repeat('abc isn cd', 100)));
var_dump(doGenerateHighlight($keywords, str_repeat('abc isn cd', 100)));
var_dump(doGenerateHighlight($keywords, str_repeat('abc isn cd', 100)));
echo microtime(true) - $time;
