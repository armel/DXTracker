// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Board
#define BOARD GREY

#define BASIC 1
#define GREY  2
#define CORE2 3

#if BOARD == BASIC
  #define M5STACK_MPU6886
  #include <M5Stack.h>
  #include "BasicAndGrey.h"
#elif BOARD == GREY
  #define M5STACK_MPU6886
  #include <M5Stack.h>
  #include "BasicAndGrey.h"
#elif BOARD == CORE2
  #include <M5Core2.h>
  #include "Core2.h"
#endif

#undef min

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <JPEGDecoder.h>
#include "font.h"
#include "settings.h"
#include "FS.h"
#include "SPIFFS.h"

// Name and Version
#define NAME "DXTracker"
#define VERSION "0.1.3"

// Wifi
WiFiClient clientHamQSL, clientSat;
WiFiClientSecure clientGreyline, clientHamQTH;

// Preferences
Preferences preferences;

// Color
typedef struct __attribute__((__packed__))
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} colorType;

colorType TFT_BACK = {48, 48, 48};
colorType TFT_GRAY = {128, 128, 128};

// HTTP endpoint
String endpointHamQSL = "http://www.hamqsl.com/solarxml.php";
String endpointSat = "http://f4hwn.globalis-dev.com/cgi-bin/DXCluster.py";
String endpointGreyline = "https://dx.qsl.net/propagation/greyline.html";
String endpointHamQTH = "https://www.hamqth.com/dxc_csv.php?limit=50";

// Scroll
TFT_eSprite imgA = TFT_eSprite(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object
String messageA = "";
int16_t posA;

TFT_eSprite imgB = TFT_eSprite(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object
String messageB = "";
int16_t posB;

// Propag data
String solarData[] = {
  "SFI", "Sunspots", "A-Index", "K-Index", 
  "X-Ray", "Helium Line", "Proton Flux", "Electron Flux", 
  "Aurora", "Solar Wind", "Magnetic Field", "Signal Noise"
};

String solarKey[] = {
  "solarflux", "sunspots", "aindex", "kindex", 
  "xray", "heliumline", "protonflux", "electonflux", 
  "aurora", "solarwind", "magneticfield", "signalnoise"
};

String skipData[] = {
  "E-Skip North America",
  "E-Skip Europe",
  "E-Skip Europe 4m",
  "E-Skip Europe 6m",
};

String skipKey[] = {
  "location=\"north_america\">", 
  "location=\"europe\">", 
  "location=\"europe_4m\">",  
  "location=\"europe_6m\">" 
};

String propagKey[] = {
  "80m-40m\" time=\"day\">", 
  "30m-20m\" time=\"day\">", 
  "17m-15m\" time=\"day\">", 
  "12m-10m\" time=\"day\">", 
  "80m-40m\" time=\"night\">",
  "30m-20m\" time=\"night\">",
  "17m-15m\" time=\"night\">",
  "12m-10m\" time=\"night\">"    
};

String cluster[50], call[50], frequency[50], band[50], country[50];

// Miscellaneous
String tmpString;
String greylineData = "", hamQSLData = "", hamQTHData = "", satData = "";
String reloadState = "";

boolean screenRefresh = 0;
boolean screensaverMode = 0;
boolean decoded;
boolean greylineRefresh = 0;

uint8_t alternance = 0;
uint8_t configCurrent = 0;
uint8_t brightnessCurrent = 128;
uint8_t messageCurrent = 0;

uint32_t screensaver;
uint32_t screensaverLimit = 30 * 60 * 1000;  // 30 minutes

int16_t parenthesisBegin = 0;
int16_t parenthesisLast = 0;

uint32_t frequencyExclude[] = {
  1840, 1842, 3573, 5357,	
  7056, 7071, 7074, 7078,
  10130, 10132, 10133, 10136, 
  14071, 14074, 14078, 14090,
  18100, 18104, 21074, 21078,
  21091, 24915, 28074, 28078,
  50310, 50313, 50328, 50323,
  70100, 144174, 222065, 432065
};