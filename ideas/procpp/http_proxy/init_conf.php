<?php

$ca_dir = '/var/wd/wrs/ca';
$env = isset($argv[1]) ? $argv[1] : 'sit';
$new_certs_dir = $ca_dir . "/cert";
$database = $ca_dir . "/index.txt";
$policy = 'policy_strict';
$serial = $ca_dir . "/serial";
$current_dir = $env == 'prod' ? '/var/wd/wrs/webroot/h5' : __DIR__;

$openssl_conf = <<<EOF
[ ca ]
default_ca = CA_default

[ CA_default ]
dir             = $ca_dir
new_certs_dir   = $new_certs_dir
database        = $database
policy          = $policy
serial          = $serial

[ policy_strict ]
commonName      = supplied

EOF;

$conf_h = <<<EOF
#pragma once

class Conf {
  public:
    static constexpr const char* CERT_DIR = "$new_certs_dir/";
    static constexpr const char* ROOT_CERT = "$current_dir/server.crt";
    static constexpr const char* DH_FILE = "$current_dir/dh2048.pem";
    static constexpr const char* ROOT_KEY = "$current_dir/server.key";
    static constexpr const char* CSR_KEY = "$current_dir/csr.key";
    static constexpr const char* OPENSSL_CONFIG = "$current_dir/openssl.conf";
};
EOF;

system("rm -rf $ca_dir/*");
system("mkdir -p $new_certs_dir");
system("touch $database");
system("echo 1000 > $serial");
file_put_contents($current_dir . "/openssl.conf", $openssl_conf);
file_put_contents($current_dir . "/conf.h", $conf_h);
