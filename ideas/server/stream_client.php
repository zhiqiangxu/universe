<?php
$fp = stream_socket_client("tcp://0.0.0.0:8000", $errno, $errstr, 30);
if (!$fp) {
  echo "$errstr ($errno)<br />\n";
}

print_r(stream_set_blocking($fp, 0));
$str = "GET / HTTP/1.0\r\nHost: www.example.com\r\nAccept: */*\r\n\r\n";
print_r(strlen($str));
print_r(fwrite($fp, $str));

while(1) {
	sleep(10);
}
