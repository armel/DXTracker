// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Multitasking task for retreive propag data and greyline 
void hamdata(void *pvParameters)
{
  for (;;)
  {
    if(alternance == 0 && reload == 0)
    {
      switch (messageCurrent)
      {
        case 0: getGreyline(); break;
        case 1: getHamSat(); getHamQTH(); break;
        case 2: getGreyline(); break;
        case 3: getHamQSL(); break;
      }
      reload = 1;
    }
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

// Get Button
void button(void *pvParameters)
{
  int8_t change;

  int8_t mapsOld = 0;
  int8_t watchOld = 0;
  int8_t beepOld = 0;
  int8_t screensaverOld = 0;
  int8_t gmtOld = 0;
  int8_t daylightOld = 0;
  uint8_t brightnessOld = 64;

  static int8_t settingsChoice = 0;
  static boolean settingsSelect = false;

  uint8_t x = 44;
  uint8_t y = 4;
  uint16_t w = 320 - (x * 2);
  uint8_t h = 185;

  for (;;)
  {
    // Get button
    getButton();

    if(btnA || btnB || btnC) {
      if(screensaverMode == 1)
      {
        Serial.println("Wake up");
        screensaverTimer = millis() - (screensaver * 60 * 1000);
        btnA = 0;
        btnB = 0;
        btnC = 0;
      }
      else {
        if (M5.Speaker.isEnabled() && beep > 0)
        {
          // set master volume (0~255)
          M5.Speaker.setVolume(beep);
          if(btnA || btnC)
          {
            // play beep sound 1000Hz 100msec (background task)
            M5.Speaker.tone(1000, 50);
          }
          else if(btnB)
          {
            // play beep sound 2000Hz 100msec (background task)
            M5.Speaker.tone(2000, 50);
          }
          // wait done
          while (M5.Speaker.isPlaying()) { vTaskDelay(1); }
        }

        screensaverTimer = millis();
        mapsOld = preferences.getUInt("maps");
        gmtOld = preferences.getInt("gmt");
        daylightOld = preferences.getUInt("daylight");
        watchOld = preferences.getUInt("watch");
        brightnessOld = preferences.getUInt("brightness");
        screensaverOld = preferences.getUInt("screensaver");
      }
    }

    if(settingsMode == false)
    {
      if(btnA || btnC)
      {
        change = alternance;
        if(btnA)
        {
          change = (change++ < 11) ? change : 0;
        }
        else if(btnC)
        {
          messageCurrent = (messageCurrent++ < 3) ? messageCurrent : 0;
          posA = 80;
          posB = 80;
        }

        if (change != alternance)
        {
          alternance = change;
        }
      }
      // Enter settings
      if(btnB) {
        settingsMode = true;
        while(settingLock == true) {
          vTaskDelay(10);
        }
        viewMenu(x, y, w, h);
        viewOption(settingsChoice, settingsSelect, x, y, w);
        vTaskDelay(300);
      }
      vTaskDelay(100);
    }
    // Select settings
    else if(settingsMode == true)
    {
      if(settingsSelect == false) {
        if(btnA || btnC) {
          if(btnA) {
            settingsChoice--;
          }
          else if(btnC) {
            settingsChoice++;
          }

          size_t stop = sizeof(settings) / sizeof(settings[0]);
          stop--;

          settingsChoice = (settingsChoice < 0) ? stop : settingsChoice;
          settingsChoice = (settingsChoice > stop) ? 0 : settingsChoice;

          viewOption(settingsChoice, settingsSelect, x, y, w);
        }
        else if(btnB) {
          settingsSelect = true;
          viewOption(settingsChoice, settingsSelect, x, y, w);

          String settingsString = String(settings[settingsChoice]);
          if(settingsString == "Shutdown") {
            shutdown();
          }
          else if(settingsString == "Exit") {
            screenRefresh = 2;
            clear();
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
        }
        vTaskDelay(pdMS_TO_TICKS(150));
      }
      // Manage settings
      else if(settingsSelect == true) {
        String settingsString = String(settings[settingsChoice]);

        display.setTextDatum(CC_DATUM);
        display.setFont(&YELLOWCRE8pt7b);
        display.setTextPadding(w - 2);
        display.setTextColor(TFT_MENU_SELECT, TFT_MENU_BACK);

        // Maps
        if(settingsString == "Maps")
        {
          display.drawString(String(choiceMaps[maps]), 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              maps -= 1;
              if(maps < 0) {
                maps = 2;
              }
            }
            else if(btnC == 1) {
              maps += 1;
              if(maps > 2) {
                maps = 0;
              }
            }
          }
          else if(btnB == 1) {
            if(mapsOld != maps)
              preferences.putUInt("maps", maps);
            screenRefresh = 2;
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // GMT Offset
        else if(settingsString == "GMT Offset")
        {
          if(gmt > 0)
          {
            display.drawString(String(choiceGMT[0]) + " +" + String(gmt), 160, h - 6);
          }
          else
          {
            display.drawString(String(choiceGMT[0]) + " " + String(gmt), 160, h - 6);
          }

          if(btnA || btnC) {
            if(btnA == 1) {
              gmt -= 1;
              if(gmt < -14) {
                gmt = -14;
              }
            }
            else if(btnC == 1) {
              gmt += 1;
              if(gmt > 14) {
                gmt = 14;
              }
            }
          }
          else if(btnB == 1) {
            if(gmtOld != gmt)
            {
              configTime(gmt * 60 * 60, daylight * 60 * 60, ntpServer);
              preferences.putInt("gmt", gmt);
            }
            screenRefresh = 2;
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(25));
        }

        // Daylight Offset
        else if(settingsString == "Daylight Offset")
        {
          display.drawString(String(choiceDaylight[0]) + " " + String(daylight), 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              daylight -= 1;
              if(daylight < 0) {
                daylight = 0;
              }
            }
            else if(btnC == 1) {
              daylight += 1;
              if(daylight > 1) {
                daylight = 1;
              }
            }
          }
          else if(btnB == 1) {
            if(daylightOld != daylight)
            {
              configTime(gmt * 60 * 60, daylight * 60 * 60, ntpServer);
              preferences.putUInt("daylight", daylight);
            }
            screenRefresh = 2;
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(25));
        }

        // watch
        if(settingsString == "Clock")
        {
          display.drawString(String(choiceClock[watch]), 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              watch -= 1;
              if(watch < 0) {
                watch = 1;
              }
            }
            else if(btnC == 1) {
              watch += 1;
              if(watch > 1) {
                watch = 0;
              }
            }
          }
          else if(btnB == 1) {
            if(watchOld != watch)
              preferences.putUInt("watch", watch);
            screenRefresh = 2;
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }

        // Brightness
        if(settingsString == "Brightness")
        {
          display.drawString(String(choiceBrightness[0]) + " " + String(brightness) + "%", 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              brightness -= 1;
              if(brightness < 1) {
                brightness = 1;
              }
            }
            else if(btnC == 1) {
              brightness += 1;
              if(brightness > 100) {
                brightness = 100;
              }
            }
          }
          else if(btnB == 1) {
            if(brightnessOld != brightness)
              preferences.putUInt("brightness", brightness);
            screenRefresh = 2;
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          setBrightness(map(brightness, 1, 100, 1, 254));
          vTaskDelay(pdMS_TO_TICKS(25));
        }
        // Beep
        else if(settingsString == "Beep")
        {
          display.drawString(String(choiceBeep[0]) + " " + String(beep) + "%", 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              beep -= 1;
              if(beep < 0) {
                beep = 0;
              }
            }
            else if(btnC == 1) {
              beep += 1;
              if(beep > 100) {
                beep = 100;
              }
            }
          }
          else if(btnB == 1) {
            if(beepOld != beep)
              preferences.putUInt("beep", beep);
            screenRefresh = 2;
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(25));
        }

        // Screensaver
        else if(settingsString == "Screensaver")
        {
          display.drawString(String(choiceScreensaver[0]) + " " + String(screensaver) + " MIN", 160, h - 6);

          if(btnA || btnC) {
            if(btnA == 1) {
              screensaver -= 1;
              if(screensaver < 1) {
                screensaver = 1;
              }
            }
            else if(btnC == 1) {
              screensaver += 1;
              if(screensaver > 60) {
                screensaver = 60;
              }
            }
          }
          else if(btnB == 1) {
            if(screensaverOld != screensaver)
              preferences.putUInt("screensaver", screensaver);
            screenRefresh = 2;
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(25));
        }

        // IP Address
        else if(settingsString == "IP Address")
        {
          display.drawString(String(WiFi.localIP().toString().c_str()), 160, h - 6);

          if(btnB == 1) {
            screenRefresh = 2;
            settingsSelect = false;
            settingsMode = false;
            vTaskDelay(pdMS_TO_TICKS(150));
          }
          vTaskDelay(pdMS_TO_TICKS(150));
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));  
  }    
}