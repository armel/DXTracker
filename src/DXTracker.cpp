// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "settings.h"
#include "DXTracker.h"
#include "map.h"
#include "font.h"
#include "tools.h"
#include "webIndex.h"
#include "data.h"
#include "functions.h"
#include "menu.h"
#include "tasks.h"

// Setup
void setup()
{
  // Init M5
  auto cfg = M5.config();
  M5.begin(cfg);

  // Init Display
  display.begin();

  // Bin Loader
  binLoader();

  // Preferences
  preferences.begin("DXTracker");
  maps = preferences.getUInt("maps", 0);
  gmt = preferences.getInt("gmt", 0);
  daylight = preferences.getUInt("daylight", 0);
  watch = preferences.getUInt("watch", 0);
  brightness = preferences.getUInt("brightness", 64);
  beep = preferences.getUInt("beep", 0);
  screensaver = preferences.getUInt("screensaver", 10);

  size_t n = sizeof(config) / sizeof(config[0]);
  n = (n / 4) - 1;

  //preferences.putUInt("config", 0);

  configCurrent = preferences.getUInt("config", 0);

  if(configCurrent > n) {
    configCurrent = 0;
    preferences.putUInt("config", configCurrent);
  }

  // LCD
  display.setBrightness(map(brightness, 1, 100, 1, 254));
  display.fillScreen(TFT_BACK);

  // SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Title
  display.setFont(&rounded_led_board10pt7b);
  display.setTextColor(TFT_WHITE, TFT_BACK);
  display.setTextDatum(CC_DATUM);
  display.drawString(String(NAME), 160, 20);
  display.setFont(0);
  display.drawString("Version " + String(VERSION) + " by F4HWN", 160, 50);

  // QRCode
  display.qrcode("https://github.com/armel/DXTracker", 90, 80, 140, 6);

  // We start by connecting to the WiFi network
  display.setTextPadding(320);

  while(true)
  {
    uint8_t attempt = 1;
    display.drawString(String(config[(configCurrent * 4)]), 160, 60);
    WiFi.begin(config[(configCurrent * 4)], config[(configCurrent * 4) + 1]);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      if(attempt % 2 == 0)
      {
        display.drawString("Connecting in progress", 160, 70);
      }
      else 
      {
        display.drawString(" ", 160, 70);
      }
      attempt++;
      if(attempt > 10) {
        break;
      }
    }
    if(WiFi.status() != WL_CONNECTED)
    {
      configCurrent += 1;
      if(configCurrent > n) {
        configCurrent = 0;
        preferences.putUInt("config", configCurrent);
      }
    }
    else {
      break;    
    }
  }

  display.drawString(String(WiFi.localIP().toString().c_str()), 160, 70);

  // Init and get time
  configTime(gmt * 60 * 60, daylight * 60 * 60, ntpServer);
  updateLocalTime();

  // Scroll
  posA = display.width();
  imgA.createSprite(display.width(), 20);

  posB = display.width();
  imgB.createSprite(display.width(), 20);

  // Start server (for Web site Screen Capture)
  httpServer.begin();     

  // Let's go after temporisation
  delay(250);

  // Waiting for data
  while(hamQSLData == "" || hamQTHData == "" || satData == "") 
  {
    display.drawString("Loading datas", 160, 70);

    if(hamQTHData == "")
    {
      getHamQTH();
      if(hamQTHData != "")
      {
        display.drawString("Cluster Ok", 160, 70);
      }
    }
    if(satData == "")
    {
      getHamSat();
      if(hamQTHData != "")
      {
        display.drawString("Sat Ok", 160, 70);
      }
    }
    if(hamQSLData == "")
    {
      getHamQSL();
      if(hamQSLData != "")
      {
        display.drawString("Solar Ok", 160, 70);
      }
    }
  }
  
  delay(500);

  for (uint8_t i = 0; i <= 120; i++)
  {
    display.drawFastHLine(0, i, 320, TFT_BLACK);
    display.drawFastHLine(0, 240 - i, 320, TFT_BLACK);
    delay(5);
  }

  // Multitasking task for retreive propag data
  xTaskCreatePinnedToCore(
      hamdata,        // Function to implement the task
      "hamdata",      // Name of the task
      16384,          // Stack size in words
      NULL,           // Task input parameter
      2,              // Priority of the task
      &hamdataHandle, // Task handle
      1);             // Core where the task should run

  // Multitasking task for retreive button
  xTaskCreatePinnedToCore(
      button,         // Function to implement the task
      "button",       // Name of the task
      8192,           // Stack size in words
      NULL,           // Task input parameter
      4,              // Priority of the task
      &buttonHandle,  // Task handle
      1);             // Core where the task should run

  // And clear
  screenRefresh = 1;

  // Init screensaver timer
  screensaverTimer = millis();
}

// Main loop
void loop()
{
  if(settingsMode == true) {
    settingLock = false;
  }
  else {
    settingLock = true;

    // Let's clean if necessary
    clear();

    // View propag datas
    title(propagData(alternance));

    // Prepare cluster and sat scroll message
    clusterAndSatMessage();

    // Prepare propag scroll message
    propagMessage();

    // Manage scroll
    scroll();

    // Manage screensaver
    wakeAndSleep();

    // Get screenshot
    getScreenshot();

    // Manage alternance
    if(screenRefresh == 0 && millis() - temporisation > TIMEOUT_TEMPORISATION) {
      temporisation = millis();
      alternance = (alternance++ > 10) ? 0 : alternance;
      if(alternance == 0) {
        messageCurrent = (messageCurrent++ < 3) ? messageCurrent : 0;
        reload = 0;
        Serial.println(String(ESP.getFreeHeap() / 1024) + " kb" + " / " + String(esp_get_minimum_free_heap_size() / 1024) + " kb");
      }
    }
  }
}
