(function() { // Comme d'habitude, une IIFE pour éviter les variables globales
    var inputs = document.getElementsByTagName('input'),
        inputsLen = inputs.length;
              
    for (var i = 0 ; i < inputsLen ; i++) {                     
    	inputs[i].addEventListener('click', function() {
            loadFile("temperature.php"); // À chaque clique, un fichier sera chargé dans la page
     	}, false);                                          
    }                                                    
})();
                                                      
function loadFile(file) {
  
      var xhr = new XMLHttpRequest();
      
          // On souhaite juste récupérer le contenu du fichier, la méthode GET suffit amplement :
              xhr.open('GET', file);
              
                  xhr.addEventListener('readystatechange', function() { // On gère ici une requête asynchrone
                  
if (xhr.readyState === 4 && (xhr.status === 200 || xhr.status === 0)) {
                          
                                      document.getElementById('bloc').innerHTML = '<span>' + xhr.responseText + '</span>'; // Et on affiche                             
         }
    }, false);
	xhr.send(null); // La requête est prête, on envoie tout !                               
}

