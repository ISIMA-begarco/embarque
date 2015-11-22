<?php
	$fichier = fopen("temperature", "r");
	
	$temperature = fgets($fichier);
	
	echo $temperature;
	
	fclose($fichier);
?>

