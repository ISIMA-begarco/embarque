<?php
	$fichier = file("history");//fopen("temperature", "r");
	//$temperature = fgets($fichier);
	echo substr($fichier[count($fichier)-1], 0, 6);
	//fclose($fichier);
?>

