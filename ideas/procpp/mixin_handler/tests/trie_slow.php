<?php

function doGenerateHighlight(Array &$keywords, $content, $caseInsensitive = false)
{
	$time = microtime(true);
    $exp = '/'
    . implode('|', array_map('preg_quote', $keywords))
    . '/' . ($caseInsensitive ? 'i' : '');
    preg_match_all($exp, $content, $matches);

    if ($matches[0]) {
        return array_unique($matches[0]);
    }

    return [];
}

class TrieNode
{
    public function __construct($value = null)
    {
        $this->value = $value;
        $this->next = [];
    }

    public function get_value()
    {
        return $this->value;
    }

    public function set_value($value)
    {
        $this->value = $value;
    }

    public function get_node($char)
    {
        return isset($this->next[$char]) ? $this->next[$char] : null;
    }

    public function add_node($char, $node)
    {
        $this->next[$char] = $node;
    }
}

class Trie
{
    public function __construct($keywords = null)
    {
        $this->root = new TrieNode;
        foreach ($keywords as $keyword) {
            if (mb_strlen($keyword) == 0) continue;

            $node = $this->root;
            $len = mb_strlen($keyword);
            for ($i = 0; $i < $len; $i++) {
                $char = mb_substr($keyword, $i, 1);
                $next = $node->get_node($char);
                if (!$next) {
                    $next = new TrieNode;
                    $node->add_node($char, $next);
                }
                $node = $next;
            }
            $node->set_value($keyword);
        }
    }

    function search($text)
    {
        $result = [];

        $len = mb_strlen($text);
        for ($i = 0; $i < $len; $i++) {

            $j = $i;
            $node = $this->root;
            do {
                $char = mb_substr($text, $j, 1);
                $node = $node->get_node($char);
                if ($node) {
                    $j ++;
                    if ($node->get_value()) {
                        $i = $j;
                        $result[$node->get_value()] = 1;
                    }
                }
            } while ($node);

        }

        return array_keys($result);
    }
}

$keywords = ['中国', '中国人', 'ab', 'bc', 'cd'];
for ($i = 0; $i < 1000; $i++) {
    $keywords[] = uniqid();
}
usort($keywords, function($a, $b) {
	return mb_strlen($b) - mb_strlen($a);
});

$s = str_repeat('中国人abc isn cd', 100);

for ($i = 0; $i < 100; $i++) {
	$time = microtime(true);

	var_dump(doGenerateHighlight($keywords, $s));

	/*
	$trie = new Trie($keywords);
	var_dump($trie->search($s));
	*/

	echo "tooks " . (microtime(true) - $time) . "\r\n";
}


