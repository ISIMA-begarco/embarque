<?php
	$lines = file("history");
	$nb = 0;
	$max = -30;
	foreach($lines as $num => $line) {
		$val = floatval(substr($line, 0, 6));
		$max = ($max > $val) ? $max : $val;
	}
	echo $max;

?>

