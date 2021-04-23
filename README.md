# DXTracker

Le projet DXTracker propose les fonctionnalités suivantes.

- Données solaires
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
- Conditions d'ouverture des bandes HF, de jour et de nuit
- Conditions d'ouverture de la bande VHF
- Etat de la Greyline
- Cluster DX

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
* votre mot de passe Wifi.

> __Remarque__
> 
> Il est possible de gérer autant de configurations que vous le souhaitez ! Cela vous permet, par exemple, de préconfigurer vos paramétrages pour une utilisation locale sur le Wifi de votre QRA et une configuration mobile sur le Wifi de votre Smartphone. Une telle configuration pourrait ressembler à ceci :
> 

```
const char *config[] = {
  "Livebox", "mdp_Livebox",
  "iPhone8", "mdp_iphone" 
};
```

# Pour finir
Compiler et uploader le projet sur votre M5Stack. C'est terminé.

# That's all
Bon trafic à tous, 88 & 73 de Armel F4HWN ! 
	
