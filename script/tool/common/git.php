<?php

function get_current_branch() {
    $output = shell_exec('git branch --no-color');
    if (!preg_match('/\*\s+(\S+)\s+/', $output, $match)) {
        exit("get_current_branch failed\n");
    }

    return $match[1];
}

function tag_exists($tag) {
    $cmd = "git rev-parse -q --verify $tag";
    return shell_exec($cmd);
}

function get_short_commit($commit) {
    $cmd = "git rev-parse --short $commit";
    return trim(shell_exec($cmd));
}

function get_remote_branch($local_branches) {
    $cmd = 'git branch -vv --no-color';
    $output = shell_exec($cmd);

    $result = [];
    foreach ($local_branches as $local_branch) {

        if (preg_match("/^\*?\s+$local_branch\s+\S+\s+\[([^\:\]]+)/m", $output, $match)) {
            $result[$local_branch] = $match[1];
        }

    }

    return $result;
}

function reset_to_remote($branches, $remote = 'origin') {
    if (!is_array($branches)) $branches = [$branches];

    $current_branch = get_current_branch();
    $remote_branches = get_remote_branch($branches);

    foreach ($branches as $branch) {
        if (empty($remote_branches[$branch])) continue;

        $remote_branch = $remote_branches[$branch];
        $cmd = $branch == $current_branch ? "git reset --hard $remote_branch" : "git branch -f $branch $remote_branch";
        shell_exec_realtime_output($cmd);
    }
}
