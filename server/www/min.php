<?php
	$lines = file("history");
	$nb = 0;
	$min = 50;
	foreach($lines as $num => $line) {
		$val = floatval(substr($line, 0, 6));
		$min = ($min < $val) ? $min : $val;
	}
	echo $min;

?>

