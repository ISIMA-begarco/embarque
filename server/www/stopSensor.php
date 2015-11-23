<?php

/* gestion des erreurs, a supprimer pour le deploiement SECURITE */
	ini_set('display_errors', 1);
		ini_set('log_errors', 1);
			ini_set('error_log', dirname(__FILE__) . '/error_log.txt');
				error_reporting(E_ALL);
					/* end erreur */

	$fichier = fopen('sensorState', "w");
	if($fichier) {
		fputs($fichier, "off");
		fclose($fichier);

		$fichier = fopen('commande', "w");
		if($fichier) {
			fputs($fichier, "Q03STOPW");
			fclose($fichier);
			header('Location: .');
		} else {
			echo "Erreur d'ouverture du fichier !";
		}

	} else {
		echo "Erreur d'ouverture du fichier !";
	}
?>

