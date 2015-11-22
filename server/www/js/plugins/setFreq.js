function chgfreq() {
	var freq = document.getElementById('slider').value;
	if(typeof freq == 'undefined') {
		document.getElementById('btn_freq').href = "setFrequence.php?freq="+document.getElementById('slider').textContent;
	} else {
		document.getElementById('btn_freq').href = "setFrequence.php?freq="+freq;
	}
}

function loadFile(file) {
     
}
