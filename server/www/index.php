<?php
	$fichier = fopen("sensorState", "r");
	$_SERVER['sensor_state'] = fgets($fichier);
	fclose($fichier);
?>

<!DOCTYPE html>
<html lang="fr">

<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="Capteur de température réalisé en 2ème Année d'école d'ingénieur">
    <meta name="author" content="Benjamin BARBESANGE, Benoît GARCON">

    <title>Capteur de température</title>

    <!-- Bootstrap Core CSS -->
    <link href="css/bootstrap.min.css" rel="stylesheet">
    <!-- Bootstrap Formulaire CSS -->
    <link href="css/bootstrap-formhelpers.min.css" rel="stylesheet">
    <!-- Custom CSS -->
    <link href="css/sb-admin.css" rel="stylesheet">
    <!-- Custom Fonts -->
    <link href="font-awesome/css/font-awesome.min.css" rel="stylesheet" type="text/css">
</head>

<body>

    <div id="wrapper">

        <!-- Navigation -->
        <nav class="navbar navbar-inverse navbar-fixed-top" role="navigation">
            <!-- Brand and toggle get grouped for better mobile display -->
            <div class="navbar-header">
                <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-ex1-collapse">
                    <span class="sr-only">Toggle navigation</span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                    <span class="icon-bar"></span>
                </button>
                <a class="navbar-brand" href="index.php">Capteur de température</a>
            </div>
            <!-- Sidebar Menu Items - These collapse to the responsive navigation menu on small screens -->
            <div class="collapse navbar-collapse navbar-ex1-collapse">
                <ul class="nav navbar-nav side-nav">
                    <li class="active">
                        <a href="index.php"><i class="fa fa-fw fa-bar-chart-o"></i> Accueil</a>
                    </li>
                </ul>
            </div>
            <!-- /.navbar-collapse -->
        </nav>

        <div id="page-wrapper">

            <div class="container-fluid">

                <!-- Page Heading -->
                <div class="row">
                    <div class="col-lg-12">
                        <h1 class="page-header">
                            Suivi de température
                        </h1>
							<p class="lead text-justify">Cette page permet de visualiser et de monitorer la température générée par un capteur fictif. Il est possible de changer la fréquence d'acquisition des données, ou encore de stopper l'ativité du capteur.</p>
							<p class="lead text-justify">Ce projet est réalisé dans le cadre d'un module de Linux embarqué de la seconde année d'étude d'ingénieur à l'ISIMA par Benjamin Barbesange et Benoît Garçon.</p>
                    </div>
                </div>
                <!-- /.row -->

				<!-- Graph temperature and commands -->
                <div class="row">
                    <div class="col-lg-8"> <!-- Graph -->
                        <div class="panel panel-primary">
                            <div class="panel-heading">
                                <h2 class="panel-title"><i class="fa fa-bar-chart-o"></i> Graphe</h2>
                            </div>
                            <div class="panel-body">
                                <div class="flot-chart">
                                    <div class="flot-chart-content" id="flot-moving-line-chart"></div>
                                </div>
                            </div>
                        </div>
                    </div>

					<div class="col-lg-4"> <!-- Command panel -->
						<div class="panel panel-primary">
							<div class="panel-heading">
								<h2 class="panel-title"><i class="fa fa-wrench"></i> Paneau d'administration</h2>
							</div>
							<div class="panel-body">
                                <div class="row">
                                    <div class="col-lg-12 text-center">
                                        <div class="panel panel-info">
                                            <div class="panel-heading">
                                                <h2 class="panel-title">Etat du capteur</h2>
                                            </div>
                                            <div class="panel-body">
                                                <div class="col-sm-6">
                                        <button type="button" class="btn <?php if($_SERVER['sensor_state'][1]=='n'){echo 'btn-success';} else { echo 'btn-danger';} ?>"><span class="glyphicon glyphicon-off"></span><?php if($_SERVER['sensor_state'][1]=="n") {echo "\tEn marche";} else {echo "\tEteint";} ?></button>
                                                </div>
                          <?php if($_SERVER['sensor_state'][1]!='n') { ?>
                                            <div class="col-sm-6">
                                        <a type="button" href="startSensor.php" class="btn btn-success">Démarrer</a>
                                                </div>
                          <?php } else { ?>
                                                <div class="col-sm-6">
                                        <a type="button" href="stopSensor.php" class="btn btn-danger">Arrêter</a>
                                                </div>
                          <?php } ?>
                                            </div>
                                        </div>
                                    </div>
                                </div> <!-- /.row -->

                                <div class="row">
                                    <div class="col-lg-12 text-center">
                                        <div class="panel panel-info">
                                            <div class="panel-heading">
                                                <h2 class="panel-title">Période d'échantillonnage (sec)</h2>
                                            </div>
                                            <div class="panel-body">
                                                <div class="col-sm-8">
                                        <div id="slider" class="bfh-slider" data-name="slider_frequency" data-min="1" data-value="<?php include('getFrequence.php'); ?>" data-max="60"><?php include('getFrequence.php'); ?></div>
                                                </div>
                                                <div class="col-sm-4">
                                        <a type="button" id="btn_freq" onclick="chgfreq()" href="setFrequence.php" class="btn btn-info"><span class="glyphicon glyphicon-refresh"></span> Set</a>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div> <!-- /.row -->
							</div> <!-- Panel body -->
						</div>
					</div> <!-- Panel commandes -->
				</div>
            </div>
                <!-- /.row -->

            </div>
            <!-- /.container-fluid -->

        </div>
        <!-- /#page-wrapper -->

    </div>
    <!-- /#wrapper -->

    <!-- jQuery -->
    <script src="js/jquery.js"></script>

    <!-- Bootstrap Core JavaScript -->
    <script src="js/bootstrap.min.js"></script>

	<!-- Bootstrap Formulaire JavaScript -->
    <script src="js/bootstrap-formhelpers.js"></script>

    <!-- Flot Charts JavaScript -->
    <!--[if lte IE 8]><script src="js/excanvas.min.js"></script><![endif]-->
    <script src="js/plugins/flot/jquery.flot.js"></script>
    <script src="js/plugins/flot/jquery.flot.tooltip.min.js"></script>
    <script src="js/plugins/flot/jquery.flot.resize.js"></script>
    <script src="js/plugins/flot/jquery.flot.pie.js"></script>
    <script src="js/plugins/flot/flot-data.js"></script>
    <script src="js/plugins/flot/getTemp.js"></script>
    <script src="js/plugins/setFreq.js"></script>

</body>

</html>
