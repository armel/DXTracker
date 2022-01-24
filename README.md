# DXTracker

Le projet DXTracker propose plusieurs fonctionnalités permettant de suivre l'activité du Soleil, les conditions d'ouverture et l'activité du trafic sur les bandes Radio Amateurs, ainsi que les prévisions de passages des satellites Radio Amateurs.

À ce titre, le DXTracker affiche en temps réel, les informations suivantes :

- Données solaires,
	- SFI,
	- Sunspots,
	- A-Index,
	- K-Index,
	- X-Ray,
	- Helium Line,
	- Proton Flux,
	- Electron Flux,
	- Aurora,
	- Solard Wind,
	- Magnetic Field,
	- Signal Noise.
- Conditions d'ouverture de la bande VHF,
- Conditions d'ouverture des bandes HF, de jour et de nuit,
- Etat de la Greyline,
- Prévisions de passages des satellites Radio Amateurs,
- Cluster DX.

![DXTracker](https://github.com/armel/DXTracker/blob/main/img/DXTracker.jpeg)


# Installation

## Pré-ambule

Le plus simple est d'installer [PlateformIO for VSCode](https://platformio.org/install/ide?install=vscode) sur votre PC (Linux ou Windows) ou sur votre Mac (Intel ou M1). C'est un environnement de développement multiplateforme et multilangage performant, en plus d'être agréable à utiliser.

## Fichier `src/DXTracker.h`

### Constante BOARD

Ligne 5, vérifier que la constante `BOARD` correspond bien à votre type de M5Stack (par défaut, la constante est initialisée à `GREY`). Donc, indiquer : 

- `GREY` si vous avez un M5Stack GREY

```
#define BOARD GREY
```

- `BASIC` si vous avez un M5Stack BASIC

```
#define BOARD BASIC
```

- `CORE2` si vous avez un M5Stack CORE2

```
#define BOARD CORE2
```
### Exclusion de fréquences

Par défaut, les fréquences FT8 sont filtrées et ne seront pas affichées par le Cluster DX. Libre à vous de modifier cette liste de filtrage et d'ajouter ou de supprimer les fréquences que vous souhaitez. Pour se faire, éditer le tableau `frequencyExclude[]`, ligne 149. 

## Fichier `platformio.ini`

### Modèle M5Core2

Si et seulement si __vous utilisez le M5Stack Core2__, éditer le fichier `platformio.ini` et modifier les lignes,

```
default_envs = m5stack-basic-grey
;default_envs = m5stack-core2
```

Par,

```
;default_envs = m5stack-basic-grey
default_envs = m5stack-core2
```

Cela revient à changer la plate-forme cible, le point-virgule étant un commentaire.

En complément, comme déjà évoqué, vérifier que vous avez bien modifié la constante `BOARD` dans le fichier `src/DXTracker.h` en indiquant :

```
#define BOARD CORE2
```

## Fichier `src/settings.h`

Editer le fichier `src/settings.h` afin de renseigner vos paramétrages, à savoir :

* votre SSID Wifi,
* votre mot de passe Wifi,
* votre latitude (format décimale, par exemple 48.8482855),
* votre longitude (format décimale, par exemple 2.2708201).

> __Remarque__
> 
> Il est possible de gérer autant de configurations que vous le souhaitez ! Cela vous permet, par exemple, de préconfigurer vos paramétrages pour une utilisation locale sur le Wifi de votre QRA et une configuration mobile sur le Wifi de votre Smartphone. Une telle configuration pourrait ressembler à ceci :
> 

```
const char *config[] = {
  "Livebox", "mdp_Livebox", "48.8482855", "2.2708201",
  "iPhone8", "mdp_iphone", "48.8482855", "2.2708201"
};
```

# Pour finir
Compiler et uploader le projet sur votre M5Stack. C'est terminé.

# Utilisation

## Mise en veille

Par défaut, le DXTracker se met automatiquement en veille au bout de 30 minutes et se reveille toutes les 30 minutes. Il reste évidemment possible de le faire sortir de veille en appuyant sur une touche du M5Stack. Cette mise en veille à pour but d'éviter une alteration prématurée de l'écran. 

## Boutons

Les boutons gauche et droit permettent de passer plus rapidement d'une donnée solaire à une autre. Le bouton central permet d'agir sur l'affichage des informations défilantes.

## Serveur Web intégré 

Cette fonctionnalité permet de visualiser votre DXTracker depuis un simple navigateur. Il est même possible de le piloter par ce biais, dans la mesure ou les boutons sont cliquables. Afin d'afficher votre DXTracker dans votre navigateur, il suffit d'aller sur `http://adresse_ip_de_votre_dxtracker/`. Pour rappel, l'adresse IP que récupère votre DXTracker s'affiche sur l'écran d'accueil, à l'allumage.

> Attention : c'est lent ! Et il n'y a pas de rafraîchissement automatique. Il faut cliquer sur le fond de l'image de l'écran pour faire une nouvelle > capture. Et sinon, comme dit, les boutons sont fonctionnels.

Mais pourquoi avoir développé une telle fonctionnalité ? Pour 2 raisons : 

- premièrement, c'était amusant à développer,
- deuxièmement, ca permet de prendre de belle capture d'écran du DXTracker, la preuve ;)

![Capture](https://github.com/armel/DXTracker/blob/main/img/screenshot.png)

# That's all
Bon trafic à tous, 88 & 73 de Armel F4HWN ! 
	
