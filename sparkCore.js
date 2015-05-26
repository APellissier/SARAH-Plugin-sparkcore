exports.action = function(data, callback, config, SARAH){
	
	// Recuperation de la configuration
	config = config.modules.sparkCore;
	if (!config.spark_apikey){
		console.log("spark_apikey non défini dans le fichier sparkCore.prop");
		callback({'tts' : 'Clé A P I non défini'});
		return;
	}
	if (!config.spark_name){
		console.log("spark_name non défini dans le fichier sparkCore.prop");
		callback({'tts' : 'Nom du spark core non défini'});
		return;
	}
	if (!config.spark_function){
		console.log("spark_function non défini dans le fichier sparkCore.prop");
		callback({'tts' : 'Nom de la fonction non défini'});
		return;
	}
    
	var request = require('request');

	if (data.ir_code != undefined){
		
		request.post({
			headers: {'content-type' : 'application/x-www-form-urlencoded', 'connection' : 'close'},
			url:     'https://api.spark.io/v1/devices/' + config.spark_name +'/' + config.spark_function + '?access_token=' + config.spark_apikey,
			method: 'POST',
			body:    "params=" + data.ir_code
			}, 
			function(error, response, body){
				//console.log(body)
				//console.log(response)
				if (error || response.statusCode != 200) {
					callback({'tts': "Erreur"});
					console.log("Erreur!")
					return;
				}
				callback({'tts' : 'Fait !'});
				console.log("Code Ir envoyé : " + data.ir_code)
			}
		);		
	}
	
	else if (data.code_433 != undefined){

		request.post({
			headers: {'content-type' : 'application/x-www-form-urlencoded', 'connection' : 'close'},
			url:     'https://api.spark.io/v1/devices/' + config.spark_name +'/' + config.spark_function433 + '?access_token=' + config.spark_apikey,
			method: 'POST',
			body:    "params=" + data.code_433
			}, 
			function(error, response, body){
				//console.log(body)
				//console.log(response)
				if (error || response.statusCode != 200) {
					callback({'tts': "Erreur"});
					console.log("Erreur!")
					return;
				}
				callback({'tts' : 'Fait !'});
				console.log("Code 433Mhz envoyé : "+data.code_433)
			}
		);
	}




	if(data.command == "temperature" || data.command == "humidite"){
		switch(data.command){
			case 'temperature':
			var url = 'https://api.spark.io/v1/devices/' + config.spark_name + '/temperature?access_token=' + config.spark_apikey;
			var isTemperature = true;
			break;
			case 'humidite':
			var url = 'https://api.spark.io/v1/devices/' + config.spark_name + '/humidite?access_token='+ config.spark_apikey;
			var isTemperature = false;
			break;
		}
		
		request({ 
			'uri' : url 
			}, 
			function (err, response, body){
				if (err || response.statusCode != 200) {
					callback({'tts': "Erreur"});
					console.log("Erreur!")
					return;
				}	
				
				var json = JSON.parse(body);
				//console.log(json.result);
			
				if (isTemperature == true){
					callback({'tts': 'La température est de' + json.result.replace('.',',') });
					console.log('La température est de ' + json.result.replace('.',','));
				}
				else {
					callback({'tts': 'L\'humidité est de' + json.result.replace('.',',') });
					console.log('L\'humidité est de ' +json.result.replace('.',','));
				}
			}
		);
	}
}