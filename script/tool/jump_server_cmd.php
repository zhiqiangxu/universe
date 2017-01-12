<?php
include __DIR__ . "/common.php";
/***************使用方法********************
 *******************************************
         php jump_server_cmd.php jump_server.cfg server.txt cmd.sh -p param
 ******************************************
 ******************************************/


$options = get_options(['p']);

$remaining_args = get_remaining_args();

list ($jump_server_config_file, $server_file, $cmd_sh_file) = $remaining_args;
$jump_server_config = require $jump_server_config_file;

$search_dir = dirname($jump_server_config_file);

if (!file_exists($server_file)) $server_file = $search_dir . "/$server_file";
$file_content = trim(file_get_contents($server_file));
$server_list = preg_split('/\s+/', $file_content);


if (!file_exists($cmd_sh_file)) $cmd_sh_file = $search_dir . "/$cmd_sh_file";

$cmd_sh = file_get_contents($cmd_sh_file);

$pids = [];
foreach ($server_list as $server) {
    echo "host $server\n";
    $pid = pcntl_fork();
    if ($pid) {
        $pids[] = $pid;
    } else {
      $random = 'justwaitcmd';
      $username = $jump_server_config['username'];
      $tzserver = $jump_server_config['tzserver'];
      $id_rsa = $jump_server_config['id_rsa'];
      $expect_cmd = <<<EOF
        set timeout 30
        spawn ssh -i $id_rsa $username@$tzserver
        expect {
        "*server ip*" {
            send "$server\r"
        }
        }
        expect {
        "*$username*" {
            send "$cmd_sh\r"
            send "echo $random \r"
         }
        }
        # Finish
        expect {
        "$random" {
        }

        }
EOF;

      $output = shell_exec("expect -c '$expect_cmd'");
      echo preg_replace("/^.*?$random\s+$/m", '', $output);
    }
}

foreach ($pids as $pid) pcntl_waitpid($pid, $status);
