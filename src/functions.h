// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Parse data
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++)
  {
    if (data.charAt(i) == separator || i == maxIndex)
    {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Compute interpolation
int interpolation(int value, int inMin, int inMax, int outMin, int outMax)
{
  if ((inMax - inMin) != 0)
  {
    return int((value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin);
  }
  else
  {
    return 0;
  }
}

// Reset color
void resetColor()
{
  switch(colorCurrent) {
    case 0: TFT_FRONT = TFT_FRONT_ROUGE; TFT_HEADER = TFT_HEADER_ROUGE; break;
    case 1: TFT_FRONT = TFT_FRONT_ORANGE; TFT_HEADER = TFT_HEADER_ORANGE; break;
    case 2: TFT_FRONT = TFT_FRONT_VERT; TFT_HEADER = TFT_HEADER_VERT; break;
    case 3: TFT_FRONT = TFT_FRONT_TURQUOISE; TFT_HEADER = TFT_HEADER_TURQUOISE; break;
    case 4: TFT_FRONT = TFT_FRONT_BLEU; TFT_HEADER = TFT_HEADER_BLEU; break;
    case 5: TFT_FRONT = TFT_FRONT_ROSE; TFT_HEADER = TFT_HEADER_ROSE; break;
    case 6: TFT_FRONT = TFT_FRONT_VIOLET; TFT_HEADER = TFT_HEADER_VIOLET; break;
    case 7: TFT_FRONT = TFT_FRONT_GRIS; TFT_HEADER = TFT_HEADER_GRIS; break;
  }
}

// Clear screen
void clear()
{
  // Reset
  message = "";
  M5.lcd.clear();
  resetColor();

  // Header
  M5.Lcd.fillRect(0, 0, 320, 44, M5.Lcd.color565(TFT_HEADER.r, TFT_HEADER.g, TFT_HEADER.b));
  M5.Lcd.drawFastHLine(  0,   0, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(  0,  44, 320, TFT_WHITE);

  // Grey zone
  M5.Lcd.drawFastHLine(  0, 100, 320, TFT_WHITE);
}

// Build scroll
void buildScroll()
{
  int h = 20;
  int w = M5.Lcd.width();

  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    img.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  img.setTextSize(1);          // Font size scaling is x1
  img.setTextFont(2);          // Font 2 selected
  //img.setFreeFont(&tahoma6pt7b);
  
  img.setTextColor(TFT_WHITE); // White text, no background colour
  img.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  img.setCursor(pos, 2); // Print text at pos
  img.print(message);

  img.setCursor(pos - w, 2); // Print text at pos - sprite width
  img.print(message);
}

// Scroll
void scroll(int pause)
{
  if(btnA == 0 && btnB == 0 && btnC == 0) {
    getButton();
  }

  // Sprite for scroll
  buildScroll();
  img.pushSprite(0, 78);

  pos -= 1;
  if (pos == 0)
  {
    pos = M5.Lcd.width();
  }

  delay(pause);
}

// Detect rotation
void getAcceleration()
{
  float accX = 0.0F;
  float accY = 0.0F;
  float accZ = 0.0F;

  if(BOARD == GREY || BOARD == CORE2) {
    M5.IMU.getAccelData(&accX,&accY,&accZ);

    if(int(accY * 1000) < -500 && M5.Lcd.getRotation() != 3) {
      M5.Lcd.setRotation(3);
    }
    else if(int(accY * 1000) > 500 && M5.Lcd.getRotation() != 1) {
      M5.Lcd.setRotation(1);
    }
  }
}

void greyline(void *pvParameters)
{
  File f;
  HTTPClient http;
  uint16_t len, check, count, httpCode;

  for (;;)
  {
    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {
      clientGreyline.setInsecure();
      http.begin(clientGreyline, endpointGreyline);   // Specify the URL
      http.setTimeout(1000);                          // Set Time Out
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
        if (httpCode == 200)                          // Check for the returning code
        {
          if (httpCode == 200) {
            // Open file
            f = SPIFFS.open("/greyline.jpg", "w+");

            // Get size
            len = http.getSize();
            check = len;
            count = 0;

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
              count += c;
            }
            Serial.println(check);
            Serial.println(count);
            Serial.println("-----");   
            if(count == check)
            {
              //SPIFFS.cp("/tmp.jpg", "/greyline.jpg");
            }     
            // Close file
            f.close();
          }
        }
      }
      http.end(); // Free the resources
    }

    decoded = JpegDec.decodeFsFile("/greyline.jpg");
    if (decoded) {
      M5.Lcd.drawJpgFile(SPIFFS, "/greyline.jpg", 0, 101, 320, 139, 0, 11, JPEG_DIV_2);
    }

    Serial.println("Ok");

    vTaskDelay(pdMS_TO_TICKS(5 * 60 * 1000));
  }
}

// Get data from HamQSL
void hamqsl(void *pvParameters)
{
  HTTPClient http;
  unsigned int limitShort = 1 * 60 * 1000; // Retry all minute
  unsigned int limitLong = 15 * 60 * 1000; // Retry all hours

  for (;;)
  {
    if ((WiFi.status() == WL_CONNECTED)) // Check the current connection status
    {
      http.begin(clientHamSQL, endpointHamQSL);     // Specify the URL
      http.addHeader("Content-Type", "text/plain");   // Specify content-type header
      http.setTimeout(1000);                          // Set Time Out
      int httpCode = http.GET();                      // Make the request
      if (httpCode == 200)                            // Check for the returning code
      {
        xmlData = http.getString(); // Get data
        http.end(); // Free the resources
        vTaskDelay(pdMS_TO_TICKS(limitLong));
      }
      else {
        http.end(); // Free the resources
        vTaskDelay(pdMS_TO_TICKS(limitShort));
      }
    }
  }
}