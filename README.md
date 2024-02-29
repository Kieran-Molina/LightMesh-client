# LightMesh-client

projet arduino IDE pour les cartes ESP8266 et ESP32.
Le code est dérivé d'un vieux projet fait à l'arrache. J'avais pas vraiment prévu de le mettre en ligne à la base. C'est pas beau, c'est pas opti, mais si ça marche...
Je ne vais pas faire de tuto pour l'IDE, google le fera mieux que moi. 

# Dependences

## Materiel
Dans `File -> Preferences -> additional board managers URL` ajouter `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
Dans l'onglet Board manager (sur la gauche) installer `esp32 by Espressif System`

sélection du matériel (board) dans l'IDE arduino: je vous ai fournis les variantes ESP32 `MH ET LIVE ESP32Minikit`, j'utilise aussi des ESP8266 `WEMOS D1 mini`

## Librairies
Les librairies à installer via l'onglet dans l'IDE :
- ArduinoJson
- AsyncTCP - si ESP32 (A remplacer par ESPAsyncTCP si ESP8266)
- FastLED
- Painless Mesh
- TaskScheduler

# Configuration
Le paramétrage se fait via le fichier `Parameters.h`.
Il faut penser à changer quelques petite choses avant de compiler et d'envoyer le code sur l'ESP:

- `MESH_PREFIX`, `MESH_PASSWORD`, `MESH_PORT` doivent correspondre entre toutes les lampes pour qu'elles se synchronisent
- `ZONE_BITMAP` permet de définir à quelle(s) zone(s) une lampe appartient
- `SHORT` 0 ou 1, en fonction de si c'est le petit modèle ou non
- `ENABLE_SYNC` 0 ou 1, active la synchro entre les lampes
- `SERVER` 0 ou 1, à activer sur un ESP qui ne sert que d'intermédiaire
- `DEFAULT_ON` 0 ou 1, définit si la lampe doit s'allumer à la mise sous tension

# Pin Layout

les pins physiques de la variante `MH ET LIVE ESP32Minikit` :
- 5v/vcc -> LED 5v
- ground -> LED ground
- IO17 -> LED data
- IO33 -> Tactile
