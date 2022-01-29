// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Multitasking task for retreive propag data and greyline
void hamdata(void *pvParameters)
{
  File f;
  HTTPClient http;
  uint32_t timer = 0, wait = 0, limit = 1 * 30 * 1000; // Retry 30 secondes
  uint16_t len, httpCode;
  static uint8_t counter = 1;
  static uint8_t counterWakeUp = 1;

  for (;;)
  {
    timer = millis();
    // If on Startup
    if(startup == 0 && (WiFi.status() == WL_CONNECTED)) {

      if(greylineData == "") {
        Serial.println("Greyline Startup");
        reloadState = "Greyline";
        clientGreyline.setInsecure();
        http.begin(clientGreyline, endpointGreyline);   // Specify the URL
        http.setTimeout(750);                          // Set Time Out
        httpCode = http.GET();                          // Make the request
        if (httpCode == 200)                            // Check for the returning code
        {
          tmpString = http.getString(); // Get data

          M5.Lcd.drawString("Greyline Preload", 160, 180);

          tmpString.replace("<img src=\"", ">>>");
          tmpString.replace("\" alt=\"Grey Line Map\"", "<<<");

          int16_t parenthesisBegin = tmpString.indexOf(">>>");
          int16_t parenthesisLast = tmpString.indexOf("<<<");

          if (parenthesisBegin > 0)
          {
            tmpString = tmpString.substring(parenthesisBegin + 4, parenthesisLast);
          }

          http.begin(clientGreyline, tmpString);      // Specify the URL
          httpCode = http.GET();                      // Make the request
          http.setTimeout(750);                       // Set Time Out
          if (httpCode == 200)                        // Check for the returning code
          {
            if (httpCode == 200) {
              greylineRefresh = 1;
              // Open file
              f = SPIFFS.open("/greyline.jpg", "w+");

              // Get size
              len = http.getSize();
              // Create buffer for read
              uint8_t buff[1024] = { 0 };

              // Get TCP stream
              WiFiClient *stream = &clientGreyline;

              // Read all data from server
              M5.Lcd.drawString("Greyline Loading", 160, 180);
              while (http.connected() && (len > 0 || len == -1)) {
                int c = stream->readBytes(buff, std::min((size_t)len, sizeof(buff)));          
                // Write it to file
                f.write(buff, c);
                if (len > 0) {
                  len -= c;
                }
              }
              // Close file
              f.close();
            }
          }
        }
        greylineData = "Ok";
        http.end(); // Free the resources
      }

      if(hamQTHData == "") {
        Serial.println("HamQTH Startup");
        reloadState = "Cluster";
        clientHamQTH.setInsecure();
        http.begin(clientHamQTH, endpointHamQTH);       // Specify the URL
        http.addHeader("Content-Type", "text/plain");   // Specify content-type header
        http.setTimeout(750);                           // Set Time Out
        httpCode = http.GET();                          // Make the request
        if (httpCode == 200)                            // Check for the returning code
        {
          hamQTHData = http.getString(); // Get data
        }
        http.end(); // Free the resources
      }

      if(hamQSLData == "") {
        Serial.println("HamQSL Startup");
        reloadState = "Solar";
        http.begin(clientHamQSL, endpointHamQSL);       // Specify the URL
        http.addHeader("Content-Type", "text/plain");   // Specify content-type header
        http.setTimeout(750);                           // Set Time Out
        httpCode = http.GET();                          // Make the request
        if (httpCode == 200)                            // Check for the returning code
        {
          hamQSLData = http.getString(); // Get data
        }
        http.end(); // Free the resources
      }

      if(satData == "") {
        Serial.println("Sat startup");
        reloadState = "Sat";
        http.begin(clientSat, endpointSat + "?lat=" + config[(configCurrent * 4) + 2] + "&lng=" + config[(configCurrent * 4) + 3] + "&format=text");       // Specify the URL
        http.addHeader("Content-Type", "text/plain");   // Specify content-type header
        http.setTimeout(2000);                          // Set Time Out
        httpCode = http.GET();                          // Make the request
        if (httpCode == 200)                            // Check for the returning code
        {
          String tmpString = http.getString(); // Get data
          tmpString.trim();

          if(tmpString != "")
          {
            satData = tmpString;
          }
        }
        http.end(); // Free the resources
      }

      counter = 2;
      vTaskDelay(pdMS_TO_TICKS(limit / 10));
    }
    // Else
    else if(startup == 1 && (WiFi.status() == WL_CONNECTED)) {

      if(counterWakeUp == 1)
      {
        screensaver = millis(); // Screensaver update !!!
      }

      if(counter == 1) // Refresh greyline only sometimes
      {
        Serial.println("Greyline");
        reloadState = "Greyline";
        clientGreyline.setInsecure();
        http.begin(clientGreyline, endpointGreyline);   // Specify the URL
        http.setTimeout(750);                           // Set Time Out
        httpCode = http.GET();                          // Make the request
        if (httpCode == 200)                            // Check for the returning code
        {
          greylineData = http.getString(); // Get data

          greylineData.replace("<img src=\"", ">>>");
          greylineData.replace("\" alt=\"Grey Line Map\"", "<<<");

          int16_t parenthesisBegin = greylineData.indexOf(">>>");
          int16_t parenthesisLast = greylineData.indexOf("<<<");

          if (parenthesisBegin > 0)
          {
            greylineData = greylineData.substring(parenthesisBegin + 4, parenthesisLast);
          }

          http.begin(clientGreyline, greylineData);     // Specify the URL
          httpCode = http.GET();                        // Make the request
          http.setTimeout(750);                         // Set Time Out
          if (httpCode == 200)                          // Check for the returning code
          {
            if (httpCode == 200) {
              // Open file
              f = SPIFFS.open("/greyline.jpg", "w+");

              // Get size
              len = http.getSize();
              // Create buffer for read
              uint8_t buff[1024] = { 0 };

              // Get TCP stream
              WiFiClient *stream = &clientGreyline;

              // Read all data from server
              while (http.connected() && (len > 0 || len == -1)) {
                int c = stream->readBytes(buff, std::min((size_t)len, sizeof(buff)));          
                // Write it to file
                f.write(buff, c);
                if (len > 0) {
                  len -= c;
                }
              }
              // Close file
              f.close();
            }
            greylineRefresh = 1;
          }
        }
        http.end(); // Free the resources
        reloadState = " ";
        vTaskDelay(pdMS_TO_TICKS(200));
      }

      if(counter == 1) // Refresh solar only sometimes
      {
        Serial.println("HamQSL");
        reloadState = "Solar";
        http.begin(clientHamQSL, endpointHamQSL);       // Specify the URL
        http.addHeader("Content-Type", "text/plain");   // Specify content-type header
        http.setTimeout(750);                           // Set Time Out
        httpCode = http.GET();                          // Make the request
        if (httpCode == 200)                            // Check for the returning code
        {
          hamQSLData = http.getString(); // Get data
        }
        http.end(); // Free the resources
        reloadState = " ";
        vTaskDelay(pdMS_TO_TICKS(200));
      }

      Serial.println("HamQTH");
      reloadState = "Cluster";
      clientHamQTH.setInsecure();
      http.begin(clientHamQTH, endpointHamQTH);       // Specify the URL
      http.addHeader("Content-Type", "text/plain");   // Specify content-type header
      http.setTimeout(750);                           // Set Time Out
      httpCode = http.GET();                          // Make the request
      if (httpCode == 200)                            // Check for the returning code
      {
        hamQTHData = http.getString(); // Get data
      }
      http.end(); // Free the resources
      reloadState = " ";
      vTaskDelay(pdMS_TO_TICKS(200));

      Serial.println("Sat");
      reloadState = "Sat";
      http.begin(clientSat, endpointSat + "?lat=" + config[(configCurrent * 4) + 2] + "&lng=" + config[(configCurrent * 4) + 3] + "&format=text");       // Specify the URL
      http.addHeader("Content-Type", "text/plain");   // Specify content-type header
      http.setTimeout(2000);                          // Set Time Out
      httpCode = http.GET();                          // Make the request
      if (httpCode == 200)                            // Check for the returning code
      {
        String tmpString = http.getString(); // Get data
        tmpString.trim();

        if(tmpString != "")
        {
          satData = tmpString;
        }
      }
      http.end(); // Free the resources
      reloadState = " ";
      vTaskDelay(pdMS_TO_TICKS(200));

      // Counter manager

      counter = (counter++ < 10) ? counter : 1;
      if(counter % 5 == 0)
      {
        int change = messageCurrent;
        change = (change++ < 3) ? change : 0;
        messageCurrent = change;
      }
      counterWakeUp = (counterWakeUp++ < 120) ? counterWakeUp : 1;
  
      // Pause
      wait = millis() - timer;

      /*
      Serial.println(counter);
      if (wait < limit) {
        Serial.println(limit - wait);
      }
      else {
        Serial.println(0);
      }
      Serial.println("----------");
      */

      if (wait < limit)
      {
        vTaskDelay(pdMS_TO_TICKS(limit - wait));
      }
    }
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

    if (M5.Lcd.getRotation() == 1) {
      right = 1;
      left = -1;
    } else {
      right = -1;
      left = 1;
    }

    if ((btnA || btnB || btnC))
    {
      screensaver = millis(); // Screensaver update !!!
      if(screensaverMode == 1)
      {
        Serial.println("Wake up");
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