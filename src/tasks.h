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

// Manage buttons
void button(void *pvParameters)
{
  int8_t right;
  int8_t left;
  int8_t change;

  for (;;)
  {
    //Serial.print(">>>button");
    //Serial.println(millis());
    getButton();

    if(buttonLeftPressed) {
      btnA = true;
      buttonLeftPressed = false;
    }
    if(buttonCenterPressed) {
      btnB = true;
      buttonCenterPressed = false;
    }
    if(buttonRightPressed) {
      btnC = true;
      buttonRightPressed = false;
    }

    /*
    if (btnA || btnB || btnC)
    {
      Serial.println(".");
      Serial.print(btnA);
      Serial.print("-");
      Serial.print(btnB);
      Serial.print("-");
      Serial.println(btnC);
    }
    else
    {
      Serial.print(".");
    }
    */

    if (display.getRotation() == 1) {
      right = 1;
      left = -1;
    } else {
      right = -1;
      left = 1;
    }

    if ((btnA || btnB || btnC))
    {
      screensaver = millis();
      if(screensaverMode == 1)
      {
        Serial.println("Wake up");
        screensaver = millis() - TIMEOUT_SCREENSAVER;
        btnA = 0;
        btnB = 0;
        btnC = 0;
      }
      else
      {
        change = alternance;
        if(btnA)
        {
          change += left;
        }
        else if(btnC)
        {
          change += right;
        }
        else if(btnB)
        {
          messageCurrent = (messageCurrent++ < 3) ? messageCurrent : 0;
          posA = 80;
          posB = 80;
        }

        change = (change < 0) ? 11 : change;
        change = (change > 11) ? 0 : change;

        if (change != alternance)
        {
          alternance = change;
          temporisation = millis(); // Temporisation update !!!
          screenRefresh = 2;
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}