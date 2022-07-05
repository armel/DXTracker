// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#define VERSION "1.2.2"
#define AUTHOR "F4HWN"
#define NAME "DXTracker"

#define DEBUG 0
#define ATOM atom

#define TIMEOUT_BIN_LOADER    3               // 3 sec
#define TIMEOUT_TEMPORISATION 5 * 1000        // 5 sec

#define WIDTH 320
#define HEIGHT 240

#define M5ATOMDISPLAY_LOGICAL_WIDTH  WIDTH    // width
#define M5ATOMDISPLAY_LOGICAL_HEIGHT  HEIGHT  // height
#define M5ATOMDISPLAY_REFRESH_RATE 60         // refresh rate

#undef min

#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Preferences.h>
#include <FS.h>
#include <SPIFFS.h>

#if ATOM == 1
  #include <M5AtomDisplay.h>
#endif

#include <M5Unified.h>
#include <M5StackUpdater.h>

// Wifi
WiFiClient client;
WiFiClient httpClient;
WiFiServer httpServer(80);
HTTPClient http;

// Web site Screen Capture stuff
#define GET_unknown 0
#define GET_index_page  1
#define GET_screenshot  2

// Display
#if ATOM == 0
  M5GFX &display(M5.Lcd);
#else
  M5AtomDisplay display(WIDTH, HEIGHT);
#endif

// Flags for button presses via Web site Screen Capture
bool buttonLeftPressed = false;
bool buttonCenterPressed = false;
bool buttonRightPressed = false;

// Preferences
Preferences preferences;

// Color
typedef struct __attribute__((__packed__))
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
} colorType;

#define TFT_BACK display.color565(48, 48, 48)
#define TFT_GRAY display.color565(128, 128, 128)
#define TFT_MENU_BORDER display.color565(115, 135, 159)
#define TFT_MENU_BACK display.color565(24, 57, 92)
#define TFT_MENU_SELECT display.color565(255, 255, 255)

// Time
const char* ntpServer = "pool.ntp.org";
char timeLocalBuff[9];
char dateLocalBuff[9];

struct tm timeinfo;

// HTTP endpoint
String endpointHamQSL = "http://www.hamqsl.com/solarxml.php";
String endpointSat = "http://rrf2.f5nlg.ovh:8080/cgi-bin/DXSat.py";
String endpointHamQTH = "http://rrf2.f5nlg.ovh:8080/cgi-bin/DXCluster.py";

String endpointGreyline[3] = {
  "http://rrf2.f5nlg.ovh:8080/greyline.jpg",
  "http://rrf2.f5nlg.ovh:8080/sunmap.jpg",
  "http://rrf2.f5nlg.ovh:8080/nightmap.jpg"
};

// Scroll
LGFX_Sprite imgA(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object
String messageA = "";
int16_t posA;

LGFX_Sprite imgB(&M5.Lcd); // Create Sprite object "img" with pointer to "tft" object
String messageB = "";
int16_t posB;

// Bin loader
File root;
String binFilename[128];
uint8_t binIndex = 0;

// Propag data
String solarData[] = {
  "SFI", "Sunspots", "A-Index", "K-Index", 
  "X-Ray", "Helium Line", "Proton Flux", "Electron Flux", 
  "Aurora", "Solar Wind", "Magnetic Field", "Signal Noise",
  ""
};

String solarKey[] = {
  "solarflux", "sunspots", "aindex", "kindex", 
  "xray", "heliumline", "protonflux", "electonflux", 
  "aurora", "solarwind", "magneticfield", "signalnoise",
  "updated"
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

String cluster, call, frequency, band, country;

// Task Handle
TaskHandle_t hamdataHandle;
TaskHandle_t buttonHandle;

// Miscellaneous
String tmpString;
String dateString;
String greylineData = "", hamQSLData = "", hamQTHData = "", satData = "";
String greylineUrl = "";
String reloadState = "";

boolean reload = 0;
boolean screensaverMode = 0;
boolean greylineRefresh = 0;
boolean settingsMode = false;
boolean settingLock = true;

uint8_t screenRefresh = 1;
uint8_t htmlGetRequest;
uint8_t alternance = 0;
uint8_t configCurrent = 0;
uint8_t brightness = 64;
uint8_t messageCurrent = 0;

int8_t beep = 0;
int8_t maps = 0;
int8_t watch = 0;
int8_t screensaver = 0;
int8_t gmt = 0;
int16_t daylight = 0;

int16_t parenthesisBegin = 0;
int16_t parenthesisLast = 0;

uint32_t temporisation;
uint32_t screensaverTimer;
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

// Menu
const char *settings[] = {"Maps", "GMT Offset", "Daylight Offset", "Clock", "Brightness", "Beep", "Screensaver", "IP Address", "Shutdown", "Exit"};
const char *choiceMaps[] = {"CLASSIC", "SUNMAP", "NIGHTMAP"};
const char *choiceGMT[] = {"GMT OFFSET"};
const char *choiceDaylight[] = {"DAYLIGHT OFFSET"};
const char *choiceBrightness[] = {"BRIGHTNESS"};
const char *choiceClock[] = {"OFF", "ON"};
const char *choiceBeep[] = {"BEEP LEVEL"};
const char *choiceScreensaver[] = {"TIMEOUT"};