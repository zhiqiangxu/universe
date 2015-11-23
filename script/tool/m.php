<?php
include __DIR__ . "/common.php";

$options = get_options(['cflags']);
$remaining_args = get_remaining_args();
$source_files = glob_files($remaining_args);

foreach($source_files as $source_file) {
	$source_content = file_get_contents($source_file);
	$input_pattern = '/{(.*?)}$/s';
	if (preg_match($input_pattern, $source_content, $matches)) {
		$matched_content = $matches[1];
		$macros = explode(';', trimlr($matched_content));
		$macro_prefix = '__NEVERCONFLICT';
		$replacement = "{{\n";
		foreach ($macros as $i => $macro) {
			if (!$macro) continue;
			$replacement .= <<<M
"$macro_prefix$i" = $macro;
M;
		}
		$replacement .= '}}';
		$modified_source_content = preg_replace($input_pattern, $replacement, $source_content);
		$cmd = 'echo ' . escapeshellarg($modified_source_content) . '|gcc -E -';
		$output = shell_exec($cmd);
		$output_pattern = '/{{(.*?)}}$/s';
		if (preg_match($output_pattern, $output, $matches)) {
			$matched_content = $matches[1];
			echo preg_replace_callback("/\"$macro_prefix(\d+)\"/", function ($m) use ($macros) { return $macros[$m[1]]; }, $matched_content) . "\n\n";
		} else {
			error_exit("output_pattern match fail for $source_file");
		}
		
	} else {
		error_exit("input_pattern match fail for $source_file");
	}
}
