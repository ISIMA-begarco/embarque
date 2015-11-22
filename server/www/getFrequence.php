<?php
	$fichier = fopen("frequence", "r");
	$freq = fgets($fichier);
	echo $freq;
	fclose($fichier);
?>

