// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Multitasking task for retreive propag data and greyline
 
void hamdata(void *pvParameters)
{
  File f;
  HTTPClient http;
  uint32_t timer = 0, wait = 0, limit = 1 * 60 * 1000; // Retry 60 secondes
  uint16_t check, httpCode;
  static uint8_t counter = 1;
  static uint8_t counterWakeUp = 1;

  for (;;)
  {
    timer = millis();
    Serial.println(counter);

    // If on Startup
    if(startup == 0 && (WiFi.status() == WL_CONNECTED)) {

      if(greylineData == "") {
        Serial.println("Greyline Startup");
        reloadState = "Greyline";
        greylineUrl = "";
        http.begin(clientGreyline, endpointGreyline);   // Specify the URL
        http.addHeader("User-Agent","M5Stack");         // Specify header
        http.addHeader("Connection","keep-alive");      // Specify header
        http.setTimeout(750);                           // Set Time Out
        check = 0;
        f = SPIFFS.open("/tmp.jpg", "w+");
        if (f) {
          int httpCode = http.GET();
          if (httpCode == 200) {
            http.writeToStream(&f);
            vTaskDelay(pdMS_TO_TICKS(100));
          }
          else {
            check = 1;
          }
          f.close();
        }
        else {
          check = 1;
        }

        http.end(); // Free the resources

        if(check == 0) {
          decoded = JpegDec.decodeFsFile("/tmp.jpg");
          if (decoded) {
            SPIFFS.remove("/greyline.jpg");
            SPIFFS.rename("/tmp.jpg", "/greyline.jpg");
            vTaskDelay(pdMS_TO_TICKS(100));
            Serial.println("Rename file");
            greylineRefresh = 1;
            greylineData = "Ok";
          }
          else {
            check = 2;
          }
        }

        if(check == 1) {
          Serial.println("HTTP Error !");
        }
        else if(check == 2) {
          Serial.println("Image Corrupt !");
        }

        vTaskDelay(pdMS_TO_TICKS(200));
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
        vTaskDelay(pdMS_TO_TICKS(200));
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
        vTaskDelay(pdMS_TO_TICKS(200));
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
        vTaskDelay(pdMS_TO_TICKS(200));
      }
      counter = 2;
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
        greylineUrl = "";
        http.begin(clientGreyline, endpointGreyline);   // Specify the URL
        http.addHeader("User-Agent","M5Stack");         // Specify header
        http.addHeader("Connection","keep-alive");      // Specify header
        http.setTimeout(500);                           // Set Time Out
        check = 0;
        f = SPIFFS.open("/tmp.jpg", "w+");
        if (f) {
          int httpCode = http.GET();
          if (httpCode == 200) {
            http.writeToStream(&f);
            vTaskDelay(pdMS_TO_TICKS(100));
          } else {
            check = 1;
          }
          f.close();        
        }
        else {
          check = 1;
        }

        http.end(); // Free the resources

        if(check == 0) {
          decoded = JpegDec.decodeFsFile("/tmp.jpg");
          if (decoded) {
            SPIFFS.remove("/greyline.jpg");
            SPIFFS.rename("/tmp.jpg", "/greyline.jpg");
            vTaskDelay(pdMS_TO_TICKS(100));
            Serial.println("Rename file");
            greylineRefresh = 1;
            greylineData = "Ok";
          }
          else {
            check = 2;
          }
        }

        if(check == 1) {
          Serial.println("HTTP Error !");
          counter = 0;
        }
        else if(check == 2) {
          Serial.println("Image Corrupt !");
          counter = 0;
        }

        reloadState = "";
      }
      else if(counter == 2) // Refresh solar only sometimes
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
        reloadState = "";
      }
      else if(counter >= 2)
      {
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
        reloadState = "";
     
        vTaskDelay(pdMS_TO_TICKS(100));

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
        reloadState = "";
      }

      // Counter manager

      counter = (counter++ < 4) ? counter : 1;
      if(counter % 2 == 0)
      {
        int change = messageCurrent;
        change = (change++ < 3) ? change : 0;
        messageCurrent = change;
      }
      counterWakeUp = (counterWakeUp++ < 20) ? counterWakeUp : 1;
  
      // Pause
      wait = millis() - timer;

      /*
      if (wait < limit) {
        Serial.println(limit - wait);
      }
      else {
        Serial.println(0);
      }
      */

      Serial.println("----------");
     
      if (wait < limit)
      {
        vTaskDelay(pdMS_TO_TICKS(limit - wait));
      }
      else {
        vTaskDelay(pdMS_TO_TICKS(100));
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
    vTaskDelay(pdMS_TO_TICKS(250));
  }
}