<?php
include __DIR__ . "/common.php";
/*****************工作流********************
 *******************************************
         1. 切分支
         git checkout -b feature_branch sit

         2. 标记起始位置
         php gerrit.php -t feature_branch sit

         3. 推送至远程分支
         git push origin HEAD

         4. 组员修改并推送
         ...

         5. 增量压扁并推送至gerrit
         php gerrit.php feature_branch sit
 ******************************************
 ******************************************/


$current_branch = get_current_branch();
$stash_output = shell_exec('git stash');

register_shutdown_function(function () use ($current_branch, $stash_output) {
    // 还原分支
    shell_exec_realtime_output("git checkout $current_branch");
    if (!preg_match('/No local changes/', $stash_output)) shell_exec_realtime_output('git stash pop');
});

$options = get_options([], ['t', 'r', 'rc']);

$remaining_args = get_remaining_args();

if ( !( $remaining_args && empty($options['t']) ) ) {
    /*
     * 1. php gerrit.php tag当前分支
     * 2. php gerrit.php -t branch [commit] tag指定分支
    */

    if (!$remaining_args) {
        $review_branch = get_current_branch();
    } else {
        $review_branch = array_shift($remaining_args);
    }

    $start_tag = "T_start_$review_branch";

    $start_commit = count($remaining_args) ? $remaining_args[0] : $review_branch;

    //tag the branch
    $cmd = "git tag -f $start_tag $start_commit";
    shell_exec_realtime_output($cmd);

    exit;
}

if ( !empty($options['rc'])  ) {

    $review_branch = $remaining_args[0];
    $start_tag = "T_start_$review_branch";

    $cmd = "git diff $start_tag $review_branch";
    shell_exec_realtime_output($cmd);

    exit;

}


/*
 * php gerrit.php review_branch merge_branch
 **/

shell_exec_realtime_output("git fetch");

list ($review_branch, $merge_branch) = $remaining_args;

$last_tag = "T_last_$review_branch" . ($merge_branch == 'sit' ? '' : "_$merge_branch");
$squash_tag = "T_squash_$review_branch";//工作分支
$start_tag = "T_start_$review_branch";

$start_commit = tag_exists($last_tag) ? $last_tag : $start_tag;

//更新目标分支
reset_to_remote([$review_branch, $merge_branch]);


if (!tag_exists($start_tag)) exit("Please tag start for $review_branch first\n");

if ( !empty($options['r'])  ) {

    $cmd = "git diff $start_commit $review_branch";
    shell_exec_realtime_output($cmd);

    exit;

}


//增量压扁
$cmd =  "git tag -f $squash_tag " . (tag_exists($last_tag) ? $last_tag : $start_tag) . " && " .
        "git checkout -q $squash_tag && " .
        "git merge --squash $review_branch && " .
        "git commit -m '$review_branch    " . get_short_commit($review_branch) . "'"
        ;

shell_exec_realtime_output($cmd);
$squash_commit = get_short_commit('HEAD');

//更新tag
$cmd =  "git tag -f $squash_tag $squash_commit && " .
        "git tag -f $last_tag $review_branch";
shell_exec_realtime_output($cmd);

//与merge_branch合并，提交至gerrit
$cmd =  "git checkout $merge_branch && " .
        "git cherry-pick $squash_tag && " .
        "git push origin HEAD:refs/for/$merge_branch";

shell_exec_realtime_output($cmd);

$review_cmd = "pushd `git rev-parse --show-toplevel` && phpcs --standard=PSR1 `git diff --name-only $start_tag $review_branch` && popd";
#shell_exec_realtime_output(bash_cmd($review_cmd));
