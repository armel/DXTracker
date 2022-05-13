// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "settings.h"
#include "DXTracker.h"
#include "map.h"
#include "font.h"
#include "tools.h"
#include "webIndex.h"
#include "functions.h"
#include "tasks.h"

// Setup
void setup()
{
  // Init screensaver timer
  screensaver = millis();

  // Init M5
  auto cfg = M5.config();
  M5.begin(cfg);

  // Init Display
  display.begin();

  // Bin Loader
  binLoader();

  // Preferences
  preferences.begin("DXTracker");

  size_t n = sizeof(config) / sizeof(config[0]);
  n = (n / 4) - 1;

  //preferences.putUInt("config", 0);

  configCurrent = preferences.getUInt("config", 0);

  if(configCurrent > n) {
    configCurrent = 0;
    preferences.putUInt("config", configCurrent);
  }

  // LCD
  display.setBrightness(brightnessCurrent);
  display.fillScreen(display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  screensaver = millis(); // Screensaver update !!!

  // SPIFFS
  if(!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Title
  display.setFont(&rounded_led_board10pt7b);
  display.setTextColor(TFT_WHITE, display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
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
  configTzTime(ntpTimeZone, ntpServer);
  updateLocalTime();

  // Scroll
  posA = display.width();
  imgA.createSprite(display.width(), 20);

  posB = display.width();
  imgB.createSprite(display.width(), 20);

  // Start server (for Web site Screen Capture)
  httpServer.begin();     

  // Clear screen
  for (uint8_t i = 0; i <= 120; i++)
  {
    display.drawFastHLine(0, i, 320, TFT_BLACK);
    display.drawFastHLine(0, 240 - i, 320, TFT_BLACK);
    delay(5);
  }

  // Select map
  greylineSelect = preferences.getUInt("map", 0);

  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextSize(1);  // Font size scaling is x1
  display.setTextFont(2);  // Font 2 selected
  display.setTextDatum(CC_DATUM);
  display.setTextPadding(160);

  display.drawString("Select Left", 80, 10);
  display.drawString("Select Right", 240, 10);

  if(greylineSelect == 0) {
      display.drawString("Current map", 80, 90);
      display.drawString("", 240, 90);
  } else {
      display.drawString("", 80, 90);
      display.drawString("Current map", 240, 90);
  }

  display.drawJpg(map_greyline, sizeof(map_greyline), 20, 20, 120, 60);
  display.drawJpg(map_sunmap, sizeof(map_sunmap), 180, 20, 120, 60);

  temporisation = millis();
  while(millis() - temporisation < TIMEOUT_MAP) {
    getButton();
    if(btnA == 1) {
      greylineSelect = 0;
      preferences.putUInt("map", greylineSelect);
      break;
    }
    if(btnC == 1) {
      greylineSelect = 1;
      preferences.putUInt("map", greylineSelect);
      break;
    }
    delay(100);   
  }

 if(greylineSelect == 0) {
      display.drawString("Current map", 80, 90);
      display.drawString("", 240, 90);
  } else {
      display.drawString("", 80, 90);
      display.drawString("Current map", 240, 90);
  }

  //Serial.println(greylineSelect);
  //Serial.println(endpointGreyline[greylineSelect]);

  // Multitasking task for retreive propag data
  xTaskCreatePinnedToCore(
      hamdata,        // Function to implement the task
      "hamdata",      // Name of the task
      16384,          // Stack size in words
      NULL,           // Task input parameter
      1,              // Priority of the task
      &hamdataHandle, // Task handle
      0);             // Core where the task should run

  // Multitasking task for retreive button
  xTaskCreatePinnedToCore(
      button,         // Function to implement the task
      "button",       // Name of the task
      8192,           // Stack size in words
      NULL,           // Task input parameter
      1,              // Priority of the task
      &buttonHandle,  // Task handle
      1);             // Core where the task should run

  // Let's go after temporisation
  delay(250);

  // Waiting for data
  display.setTextColor(TFT_WHITE, TFT_BLACK);
  display.setTextSize(1);  // Font size scaling is x1
  display.setTextFont(2);  // Font 2 selected
  display.setTextDatum(CC_DATUM);
  display.setTextPadding(320);

  while(greylineData == "" || hamQSLData == "" || hamQTHData == "" || satData == "") 
  {
    display.drawString("Loading datas", 160, 110);
    delay(250);
    display.drawString(" ", 160, 110);
    delay(250);
    display.drawString("It takes a while, so please wait !", 160, 130);

    if(greylineData != "")
    {
      display.drawString("Greyline Ok", 160, 170);
    }
    if(hamQSLData != "")
    {
      display.drawString("Solar Ok", 160, 190);
    }
    if(hamQTHData != "")
    {
      display.drawString("Cluster Ok", 160, 210);
    }
    if(satData != "")
    {
      display.drawString("Sat Ok", 160, 230);
    }
  }

  startup = 1;
  
  delay(500);

  for (uint8_t i = 0; i <= 120; i++)
  {
    display.drawFastHLine(0, i, 320, TFT_BLACK);
    display.drawFastHLine(0, 240 - i, 320, TFT_BLACK);
    delay(5);
  }

  // And clear
  screenRefresh = 1;
}

// Main loop
void loop()
{
  // Let's clean if necessary
  clear();

  // View propag datas
  propagData();

  // Prepare cluster and sat scroll message
  clusterAndSatMessage();

  // Prepare propag scroll message
  propagMessage();

  // View greyline
  greyline();

  // Manage scroll
  scroll();

  // Manage Web site Screen Capture
  getScreenshot();

  // Manage screensaver
  wakeAndSleep();

  // Manage alternance
  if(screenRefresh == 0 && millis() - temporisation > TIMEOUT_TEMPORISATION) {
    temporisation = millis();
    alternance++;
    alternance = (alternance > 11) ? 0 : alternance;
  }
}