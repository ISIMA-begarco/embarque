<?php

/* gestion des erreurs, a supprimer pour le deploiement SECURITE */
	ini_set('display_errors', 1);
		ini_set('log_errors', 1);
			ini_set('error_log', dirname(__FILE__) . '/error_log.txt');
				error_reporting(E_ALL);
					/* end erreur */

	$fichier = fopen('frequence', "w");
	if($fichier) {
		fputs($fichier, $_GET['freq']);
		fclose($fichier);
		header('Location: .');
	} else {
		echo "Erreur d'ouverture du fichier !";
	}
?>

