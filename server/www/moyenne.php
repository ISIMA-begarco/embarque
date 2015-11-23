<?php
	$lines = file("history");
	$nb = 0;
	$somme = 0;
	foreach($lines as $num => $line) {
		$nb++;
		$somme += floatval(substr($line, 0, 6));
	}
	if($nb!=0) {
		echo substr(strval($somme/$nb), 0, 6);
	}

?>

