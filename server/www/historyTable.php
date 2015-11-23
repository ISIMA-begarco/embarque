<?php
	$lines = file("history");
	
	foreach($lines as $num => $line) {
		$temp = substr($line, 0, 6);
		$date = substr($line, 12, 2)."/".substr($line, 14,2)."/".substr($line, 16, 2)." à ".substr($line, 7, 2)."h".substr($line, 9, 2);
		echo "<tr><td>".($num+1)."</td><td>".$date."</td><td>".$temp."</td></tr>";
	}

?>

