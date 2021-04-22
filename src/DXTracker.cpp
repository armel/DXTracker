// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "DXTracker.h"
#include "functions.h"
#include "tasks.h"

// Setup
void setup()
{
  // Debug
  Serial.begin(115200);

  // Init screensaver timer
  screensaver = millis();

  // Init M5
  M5.begin(true, false, false, false);
  power();

  // Preferences
  preferences.begin("SolarMonitor");

  size_t n = sizeof(config) / sizeof(config[0]);
  n = (n / 6) - 1;

  //preferences.putUInt("config", 0);

  configCurrent = preferences.getUInt("config", 0);

  if(configCurrent > n) {
    configCurrent = 0;
    preferences.putUInt("config", configCurrent);
  }

  // LCD
  M5.Lcd.setBrightness(64);
  M5.Lcd.fillScreen(M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

  // SPIFFS
  SPIFFS.begin(true);
  if(!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Title
  M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.drawString("DXTracker", 160, 20);
  M5.Lcd.setFreeFont(0);
  M5.Lcd.drawString("Version " + String(VERSION) + " par F4HWN", 160, 50);

  // QRCode
  M5.Lcd.qrcode("https://github.com/armel/DXTracker", 90, 80, 140, 6);

  // We start by connecting to the WiFi network
  M5.Lcd.setTextPadding(320);

  while(true)
  {
    uint8_t attempt = 1;
    M5.Lcd.drawString(String(config[(configCurrent * 6)]), 160, 60);
    WiFi.begin(config[(configCurrent * 6)], config[(configCurrent * 6) + 1]);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      if(attempt % 2 == 0)
      {
        M5.Lcd.drawString("Connexion en cours", 160, 70);
      }
      else 
      {
        M5.Lcd.drawString(" ", 160, 70);
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

  M5.Lcd.drawString(String(WiFi.localIP().toString().c_str()), 160, 70);

  // Scroll
  posV = M5.Lcd.width();
  imgV.createSprite(M5.Lcd.width(), 20);

  posH = M5.Lcd.width();
  imgH.createSprite(M5.Lcd.width(), 20);

  // Multitasking task for retreive rrf, spotnik and propag data
  xTaskCreatePinnedToCore(
      hamdata,      // Function to implement the task
      "hamdata",    // Name of the task
      16384,        // Stack size in words
      NULL,         // Task input parameter
      1,            // Priority of the task
      NULL,         // Task handle
      1);           // Core where the task should run

  xTaskCreatePinnedToCore(
      button,       // Function to implement the task
      "button",     // Name of the task
      8192,         // Stack size in words
      NULL,         // Task input parameter
      2,            // Priority of the task
      NULL,         // Task handle
      1);           // Core where the task should run

  // Accelelerometer
  M5.IMU.Init();

  // Let's go after temporisation
  delay(3000);

  for (uint8_t i = 0; i <= 120; i++)
  {
    M5.Lcd.drawFastHLine(0, i, 320, TFT_BLACK);
    M5.Lcd.drawFastHLine(0, 240 - i, 320, TFT_BLACK);
    delay(5);
  }
}

// Main loop
void loop()
{  
  // Let's clean
  clear();

  // Propag data and message
  propagMessage();
  clusterMessage();
  greyline();
  
  if(btnB)
  {
    propagCondition();
  }
  else
  {
    propagData();
    temporisation();
  }

  // Manage refresh and alternance
  if(screenRefresh != 1)
  {
    alternance++;
    alternance = (alternance > 11) ? 0 : alternance;
  }
  else
  {
    screenRefresh = 0;
  }
}