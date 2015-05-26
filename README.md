# SARAH-Plugin-sparkcore
Ce plug-in vous permet de commander des périphériques infrarouges ainsi que des prises activables à distance en 433 Mhz que l'on trouve couramment dans les supermarchés et autres magasin de bricolage. Il permet aussi de connaitre la température intérieur ainsi que l'humidité votre pièce.

Prerequis

Un SparkCore
Un module récepteur 433Mhz
Un module émetteur 433Mhz
Un émetteur inrarouge
Un récepteur infrarouge
Un capteur de température/humidité DHT22/DHT11/DHT21
Câblage

Vous trouverez toutes le infos necesaire sur internet, voici quelques liens :

DHT22

https://raygun.io/blog/2014/09/home-automation-the-easy-way/

Emmeteur IR

http://www.blackcj.com/blog/2014/08/10/control-an-ir-device-with-a-spark-core/

Récepteur infrarouge

http://www.seeedstudio.com/wiki/Grove_-_Infrared_Receiver

Recepteur Emmeteur 433Mhz

http://arduinobasics.blogspot.fr/2014/07/433-mhz-rf-module-with-arduino-tutorial_30.html

 

Code sparkCore

Rendez-vous sur l'IDE sparkcore, créez un nouveau projet et nommez le comme vous voulez.

Copiez le code contenu dans le fichier skecth/sparkCore.ino dans votre nouveau projet.

Ajoutez une nouvel page à votre projet en cliquant sur le petit plus en haut a droite et nommez le IRremote, ceci va créer deux nouveaux onglets IRremote.cpp et IRremote.h

Copiez le contenu sketch/IRremote.h dans IRremote.h et faite de même pour IRremote.cpp

Ajoutez ensuite la bibliothèque ADAFRUIT_DHT ainsi que la bibliothèque RCSWITCH à votre projet et flashez le tout.

 

Réception des codes

En premier lieu, vous devez noter les codes de vos périphériques en vous rendant sur

https://api.spark.io/v1/devices/LE_NOM_DE_VOTRE_PERIPHERIQUE/events/?access_token=VOTRE_CLE_API 
A chaque commande reçue vous verrez sur la page un code de cette forme :

Pour l'inrarouge :

event: ReceptionIR  data: {"data":"4294967295","ttl":"5","published_at":"2015-05-26T12:40:10.566Z","coreid":"xxxxxxxxxxxxxxxx"}  

Pour le 433Mhz :

event: Reception433Mhz  data: {"data":"414 FFF0FFFFFFF1","ttl":"60","published_at":"2015-05-26T12:26:42.209Z","coreid":"xxxxxxxxxxxxxxxx"} 
Vous devrez noter le code de chacune des fonctions que vous voulez utiliser

Attention : il y a un bug que je ne comprend pas dans mon programme, la réception des codes infrarouge ne fonctionne que lors du démarrage de la carte. Si vous commandez un périphériques avec sarah, la réception ne fonctionnera plus et il faudra redémarrer la carte pour pouvoir noter les codes.

 

Utilisation avec SARAH

Configuration

Ouvrez le fichier sparkCore.prop et renseignez le champ "spark_name" et "spark_apikey" par le nom de votre spark core et votre clé API.

Ensuite ouvrez le fichier sparkCore.xml et modifiez les commandes comme vous le souhaitez avec les commandes ci-dessous.

Infrarouge

Les commandes infrarouges s'envoient avec cette commande:

<item>Met TF1<tag>out.action.ir_code="CODE_INFRAROUGE";</tag></item>
Pour envoyer plusieurs commandes à la fois, il suffit de mettre un tiret après chaque codes (le code en supporte 3 au maximum) ex :

<item>Allume la télé et l'ampli<tag>out.action.ir_code="CODE1-CODE2";</tag></item>
433Mhz

<item>Allume la lumiere<tag>out.action.code_433="FFF00FFFFFFF";</tag></item>
Température/humidité

<item>Donne moi la température<tag>out.action.command = "temperature";</tag></item>

<item>Donne moi l'humidté<tag>out.action.command="humidite";</tag></item>
