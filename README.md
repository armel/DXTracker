# DXTracker

Le projet DXTracker propose plusieurs fonctionnalités permettant de suivre l'activité du Soleil, les conditions d'ouverture et l'activité du trafic sur les bandes Radio Amateurs.

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
- Conditions d'ouverture des bandes HF, de jour et de nuit,
- Conditions d'ouverture de la bande VHF,
- Etat de la Greyline,
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

Par défaut, les fréquences FT8 sont filtrées et ne seront pas affichées par le Cluster DX. Libre à vous de modifier cette liste de filtrage et d'ajouter ou de supprimer les fréquences que vous souhaitez. Pour se faire, éditer le tableau `frequencyExclude[]`, ligne 130. 

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

Les boutons gauche et droit permettent de passer plus rapidement d'une données solaires à une autre. Le bouton central permet d'afficher l'état d'ouverture des bandes HF.

# That's all
Bon trafic à tous, 88 & 73 de Armel F4HWN ! 
	
