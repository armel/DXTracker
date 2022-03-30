# DXTracker
![basic](https://img.shields.io/badge/M5Stack-BASIC-blue)
![grey](https://img.shields.io/badge/M5Stack-GREY-blue)
![fire](https://img.shields.io/badge/M5Stack-FIRE-orange)
![core2](https://img.shields.io/badge/M5Stack-CORE2-green)
![aws](https://img.shields.io/badge/M5Stack-AWS-orange)

![licence](https://img.shields.io/github/license/armel/DXTracker)
![language](https://img.shields.io/github/languages/top/armel/DXTracker)
![size](https://img.shields.io/github/repo-size/armel/DXTracker)
![version](https://img.shields.io/github/v/release/armel/DXTracker)
![activity](https://img.shields.io/github/commit-activity/y/armel/DXTracker)

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

## Utilisation du Bin Loader (_power user only..._)

Evolution récente de mes développements, il est désormais possible de stocker plusieurs applications dans la mémoire Flash de votre M5Stack. Au démarrage, une procédure est prévue pour charger une application en particulier.

### Préparation

Je vais détailler ici la procédure pour déployer l'application RRFRemote et DXTRacker sur un même M5Stack.

#### Etape 1 - Compilation

Commencez par compiler vos applications, comme vous aviez l'habitude de le faire. Rien ne change ici. Par exemple, commencez par compiler l'application RRFRemote. Puis faites de même avec l'application DXTracker. 

> À noter que l'application 705SMeter dispose aussi du Bin Loader. Mais elle n'a d'intérêt que si vous disposez d'un IC 705.

#### Etape 2 - Collecte des fichiers binaires

Ca y est, vous avez compiler l'application RRFRemote et DXTracker ? C'est parfait.

Chaque compilation a produit un binaire. C'est ce binaire qui est envoyé / flashé sur votre M5Stack, via la connexion USB.

Placez vous à la racine du dossier RRFRemote, qui contient l'ensemble du projet. Et allez dans le répertoire :

- `.pio/build/m5stack-basic-grey`, si vous avez compilé pour un M5Stack GREY ou BASIC
- `.pio/build/m5stack-core2`, si vous avez compilé pour un M5Stack CORE2 ou AWS

Vous y trouverez un fichier `firmware.bin`. Copier le dans le répertoire `data` qui se trouve à la racine du dossier RRFRemote. Et profitez en pour le renommer en l'appelant, par exemple, `RRFRemote.bin`.

> Si le dossier `data` n'existe pas, créé le.

Faites de même avec l'application DXTracker. Placez vous à la racine du dossier DXTracker, qui contient l'ensemble du projet. Et allez dans le répertoire :

- `.pio/build/m5stack-basic-grey`, si vous avez compilé pour un M5Stack GREY ou BASIC
- `.pio/build/m5stack-core2`, si vous avez compilé pour un M5Stack CORE2 ou AWS

Vous y trouverez également un fichier `firmware.bin`. Copier le, lui aussi, dans le répertoire `data` qui se trouve à la racine du dossier RRFRemote. Et profitez en pour le renommer en l'appelant, par exemple, `DXTracker.bin`.

> **Hyper important, l'idée est bien de copier ces 2 binaires dans le même répertoire `data`** (situé à la racine du dossier RRFRemote).

A ce stade, vous devez donc avoir 2 fichiers binaires clairement identifiés : `RRFRemote.bin` et `DXTracker.bin` dans le répertoire `data` qui se trouve à la racine du dossier RRFRemote.

#### Etape 3 - Copie dans la mémoire Flash du M5Stack

Passons à l'étape probablement la plus compliquée. Ouvrez le projet RRFRemote depuis Visual Studio Code, comme vous le feriez pour le compiler. 

![Capture](https://github.com/armel/DXTracker/blob/main/img/flash_1.png)

Etape 1, cliquez sur l'icône Platformio (l'icone avec une tête de fourmi...). Etape 2, déroulez la section :

- `m5stack-basic-grey`, si vous avez compilé pour un M5Stack GREY ou BASIC
- `m5stack-core2`, si vous avez compilé pour un M5Stack CORE2 ou AWS

![Capture](https://github.com/armel/DXTracker/blob/main/img/flash_2.png)

Enfin, étape 3, allez dans la sous section `Platform`. Et cliquez sur `Upload Filesystem Image`.

Patientez. Le contenu du répertoire `data` va être écrit dans la mémoire Flash de votre M5Stack. Ca y est ? Vous y êtes !!!!

### Utilisation

Démarrez votre M5Stack. Vous devriez voir un écran noir, suivi de l'affichage de 1, 2 puis 3 petits points, en haut de l'écran. C'est le fameux Bin Loader ;)

Dès l'affichage du premier petit point, vous pouvez :

- soit appuyez sur le bouton gauche ou droite, pour lancer l'application par défaut.
- soit appuyez sur le bouton central. Dans ce cas, le menu du Bin Loader s'affiche et vous propose la liste des binaires disponnibles en mémoire Flash. 

Si vous avez parfaitement suivi la procédure, vous devriez avoir le choix entre `RRFRemote.bin` et `DXTracker.bin`.

Les boutons gauche et droite, permettent de passer d'un binaire à un autre. Et le bouton central permet de valider le binaire en cours de sélection. Dans ce cas, l'application selectionnée sera chargée ;)

> Le chargement prend environ 20 secondes. C'est supportable.

### Limitation

Je pense qu'il est possible de faire cohabiter 5 ou 6 applications, dans la mémoire Flash de votre M5Stack. En l'état, ca me semble suffisant. Si besoin, j'adapterais le code pour les binaires sur la carte SD.  

# Donations
If you find this app fun and useful then [offer me a beer](https://www.paypal.me/F4HWN) :)

# That's all
Bon trafic à tous, 88 & 73 de Armel F4HWN ! 
	
