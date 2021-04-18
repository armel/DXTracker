// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "SolarMonitor.h"
#include "functions.h"

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

  // Init Speaker
  speaker();

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

  brightnessCurrent = preferences.getUInt("brightness", 128);
  totCurrent = preferences.getUInt("tot", 0);

  // LCD
  resetColor();
  M5.Lcd.setBrightness(brightnessCurrent);
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
  M5.Lcd.drawString("Solar Monitor", 160, 20);
  M5.Lcd.setFreeFont(0);
  M5.Lcd.drawString("Version " + String(VERSION) + " par F4HWN", 160, 50);

  // QRCode
  M5.Lcd.qrcode("https://github.com/armel/SolarMonitor", 90, 80, 140, 6);

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
  pos = M5.Lcd.width();
  img.createSprite(M5.Lcd.width(), 20);

  // Multitasking task for retreive rrf, spotnik and propag data
  xTaskCreatePinnedToCore(
      hamqsl,       // Function to implement the task
      "hamqsl",     // Name of the task
      8192,         // Stack size in words
      NULL,         // Task input parameter
      1,            // Priority of the task
      NULL,         // Task handle
      1);           // Core where the task should run

  xTaskCreatePinnedToCore(
      greyline,     // Function to implement the task
      "greyline",   // Name of the task
      8192,         // Stack size in words
      NULL,         // Task input parameter
      2,            // Priority of the task
      NULL,         // Task handle
      1);           // Core where the task should run

  // Accelelerometer
  M5.IMU.Init();

  // Let's go after temporisation
  delay(1000);

  for (uint8_t i = 0; i <= 120; i++)
  {
    M5.Lcd.drawFastHLine(0, i, 320, TFT_BLACK);
    M5.Lcd.drawFastHLine(0, 240 - i, 320, TFT_BLACK);
    delay(5);
  }

  // Draw first
  decoded = JpegDec.decodeFsFile("/greyline.jpg");

  if (decoded) {
    M5.Lcd.drawJpgFile(SPIFFS, "/greyline.jpg", 0, 101, 320, 139, 0, 11, JPEG_DIV_2);
  }
}

// Main loop
void loop()
{  
  String solarTitle[] = {"SFI", "Sunspots", "A-Index", "K-Index", "X-Ray", "Helium Line", "Proton Flux", "Electron Flux", "Aurora", "Solar Wind", "Magnetic Field", "Signal Noise"};
  String solarKey[] = {"solarflux", "sunspots", "aindex", "kindex", "xray", "heliumline", "protonflux", "electonflux", "aurora", "solarwind", "magneticfield", "signalnoise"};
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
  
  uint16_t i;
  
  M5.Lcd.fillRect(0, 0, 320, 44, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastHLine(  0, 0, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(  0, 44, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(  0, 100, 320, TFT_WHITE);

  // Title
  solarTitle[alternance].toUpperCase();
  title(solarTitle[alternance]);

  // Current value
  tmpString = xmlData;
  tmpString.replace("<" + solarKey[alternance] + ">", "(");
  tmpString.replace("</" + solarKey[alternance] + ">", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")");
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextPadding(320);

  M5.Lcd.drawString(tmpString, 160, 60);

  // Current propagation 50 MHz
  tmpString = xmlData;
  tmpString.replace("location=\"europe_6m\">", "(");
  tmpString.replace("</phenomenon>", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  message = "E-Skip / 50 Mhz / " + tmpString;

  alternance++;
  if(alternance == 12) {
    alternance = 0;
  }

  for(i = 0; i <= 500; i += 1)
  {
    scroll(10);
  }

  if(alternance == 5 || alternance == 11)
  {
    for(i = 0; i < 56; i += 1)
    {
      M5.Lcd.drawFastHLine(  0, 44 + i, 320, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawFastHLine(  0, 44 + i + 1, 320, TFT_WHITE);
      if (i < 28)
      {
        scroll(10);
      }
      else
      {
        delay(10);
      }
    }

    title("BANDS CONDITIONS");

    // Current propagation
    M5.Lcd.setFreeFont(&DroidSansMono6pt7b);
    M5.Lcd.setTextPadding(160);
    M5.Lcd.setTextDatum(CL_DATUM);

    // Day
    for(i = 0; i <= 3; i += 1)
    {
      tmpString = xmlData;
      tmpString.replace(propagKey[i], "(");
      tmpString.replace("</band>", ")");
      parenthesisBegin = tmpString.indexOf("(");
      parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
      if (parenthesisBegin > 0)
      {
        tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
      }
      tmpString.trim();
      tmpString.toUpperCase();

      M5.Lcd.drawString(propagKey[i].substring(0, 7) + " DAY " + tmpString, 20, 48 + (14 * i));
    }

    M5.Lcd.setTextDatum(CR_DATUM);

    // Night
    for(i = 4; i <= 7; i += 1)
    {
      tmpString = xmlData;
      tmpString.replace(propagKey[i], "(");
      tmpString.replace("</band>", ")");
      parenthesisBegin = tmpString.indexOf("(");
      parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
      if (parenthesisBegin > 0)
      {
        tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
      }
      tmpString.trim();
      tmpString.toUpperCase();

      M5.Lcd.drawString(propagKey[i].substring(0, 7) + " NIGHT " + tmpString, 300, 48 + (14 * (i - 4)));
    }

    for(i = 0; i <= 500; i += 1)
    {
      delay(10);
    }

    for(i = 0; i < 56; i += 1)
    {
      M5.Lcd.drawFastHLine(  0, 99 - i, 320, TFT_BLACK);
      M5.Lcd.drawFastHLine(  0, 99 - i - 1, 320, TFT_WHITE);
      if (i > 28)
      {
        scroll(10);
      }
      else
      {
        delay(10);
      }
    }
  }
  else {
    for(i = 0; i <= 500; i += 1)
    {
      scroll(10);
    }
  }
}