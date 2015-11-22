<?php

/* gestion des erreurs, a supprimer pour le deploiement SECURITE */
	ini_set('display_errors', 1);
		ini_set('log_errors', 1);
			ini_set('error_log', dirname(__FILE__) . '/error_log.txt');
				error_reporting(E_ALL);
					/* end erreur */

	$fichier = fopen('sensorState', "w");
	if($fichier) {
		fputs($fichier, "on");
		fclose($fichier);
/*
		$connection = ssh2_connect('192.168.1.20', 22);
		$auth = @ssh2_auth_password($connection, 'username', 'password');
		$stream = ssh2_exec($connection, 'cd /var/www/garcon/embarque; nohup ./testJS &;');
		unset($connection);
*/
		system("./startSensor.sh");
		header('Location: .');
	} else {
		echo "Erreur d'ouverture du fichier !";
	}
?>

