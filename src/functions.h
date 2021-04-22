// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// Parse data
String getValue(String data, char separator, uint16_t index)
{
  int16_t strIndex[] = {0, -1};
  int16_t found = 0;
  int16_t maxIndex = data.length() - 1;

  for (uint16_t i = 0; i <= maxIndex && found <= index; i++)
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

// Clear screen
void clear()
{
  M5.Lcd.fillRect(0, 0, 320, 44, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.drawFastHLine(  0, 0, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(  0, 44, 320, TFT_WHITE);
  M5.Lcd.drawFastHLine(  0, 100, 320, TFT_WHITE);
}

// Build scroll A
void buildScrollA()
{
  int16_t h = 20;
  int16_t w = M5.Lcd.width() * 8;

  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    imgA.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  imgA.setTextSize(1);          // Font size scaling is x1
  imgA.setTextFont(2);          // Font 2 selected
  
  imgA.setTextColor(TFT_WHITE); // White text, no background colour
  imgA.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  imgA.setCursor(posA, 2); // Print text at pos
  imgA.print(messageA);

  imgA.setCursor(posA - w, 2); // Print text at pos - sprite width
  imgA.print(messageA);
}

// Scroll A
void scrollA(uint8_t pause)
{
  // Sprite for scroll
  buildScrollA();
  imgA.pushSprite(0, 52);

  posA -= 1;
  if (posA == 0)
  {
    posA = M5.Lcd.width() * 8;
  }

  delay(pause);
}

// Build scroll B
void buildScrollB()
{
  int16_t h = 20;
  int16_t w = M5.Lcd.width() * 2;

  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    imgB.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  imgB.setTextSize(1);          // Font size scaling is x1
  imgB.setTextFont(2);          // Font 2 selected
  
  imgB.setTextColor(M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b)); // Gray text, no background colour
  imgB.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  imgB.setCursor(posB, 2); // Print text at pos
  imgB.print(messageB);

  imgB.setCursor(posB - w, 2); // Print text at pos - sprite width
  imgB.print(messageB);
}

// Scroll B
void scrollB(uint8_t pause)
{
  // Sprite for scroll
  buildScrollB();
  imgB.pushSprite(0, 74);

  posB -= 1;
  if (posB == 0)
  {
    posB = M5.Lcd.width() * 2;
  }

  delay(pause);
}

// Manage temporisation
void temporisation()
{
  for(uint16_t i = 0; i <= 500; i += 1)
  {
    if(screenRefresh == 1)
    {
      break;
    }
    scrollA(5);
    scrollB(5);
  }
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

// Draw title
void title(String title)
{
  // Title
  M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
  M5.Lcd.setFreeFont(&dot15pt7b);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextPadding(320);
  M5.Lcd.drawString(title, 160, 16);

  M5.Lcd.setFreeFont(0);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextPadding(320);

  if(alternance % 2 == 0)
  {
    // Update date and time
    tmpString = hamQSLData;
    tmpString.replace("<updated>", "(");
    tmpString.replace("</updated>", ")");
    parenthesisBegin = tmpString.indexOf("(");
    parenthesisLast = tmpString.indexOf(")");
    if (parenthesisBegin > 0)
    {
      tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
    }

    tmpString.trim();
  }
  else
  {
    tmpString = String(NAME) + " Version " + String(VERSION);
  }

  M5.Lcd.drawString(tmpString, 160, 36);
}

// Draw Propag Data
void propagData()
{
  // Title
  solarData[alternance].toUpperCase();

  // Current value
  tmpString = hamQSLData;
  tmpString.replace("<" + solarKey[alternance] + ">", "(");
  tmpString.replace("</" + solarKey[alternance] + ">", ")");

  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")");
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  title(solarData[alternance] + " " + tmpString);
}

// Draw Propag Message
void propagMessage()
{
  messageB = "";
  // Current propagation 50 MHz
  tmpString = hamQSLData;
  tmpString.replace("location=\"europe\">", "(");
  tmpString.replace("</phenomenon>", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  messageB += "E-Skip Europe " + tmpString;

  messageB += " -- ";

  // Current propagation 50 MHz
  tmpString = hamQSLData;
  tmpString.replace("location=\"europe_6m\">", "(");
  tmpString.replace("</phenomenon>", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  messageB += "E-Skip Europe 6m " + tmpString;

  messageB += " -- ";

  // Current propagation 70 MHz
  tmpString = hamQSLData;
  tmpString.replace("location=\"europe_4m\">", "(");
  tmpString.replace("</phenomenon>", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  messageB += "E-Skip Europe 4m " + tmpString;
}

// Draw Cluster Message
void clusterMessage()
{
  boolean exclude = 0;
  uint8_t counter = 0;
  uint32_t tmp = 0;

  size_t n = sizeof(frequencyExclude)/sizeof(frequencyExclude[0]);

  messageA = "";
  hamQTHData.replace("\n", "|");

  for (uint8_t i = 0; i < 50; i++)
  {
    cluster[i] = getValue(hamQTHData, '|', i);
    frequency[i] = getValue(cluster[i], '^', 1);
    tmp = frequency[i].toInt();
    
    exclude = 0;

    for (uint8_t j = 0; j < n; j++)
    {
      if(abs(tmp - frequencyExclude[j]) <= 2)
      {
        exclude = 1;
        break;
      }
    }

    if(exclude == 0)
    {    
      call[i] = getValue(cluster[i], '^', 0);
      band[i] = getValue(cluster[i], '^', 8);
      country[i] = getValue(cluster[i], '^', 9);

      messageA += call[i] + " " + band[i] + " " + frequency[i] + " " + country[i] + " -- ";
      counter += 1;
    }

    if(counter == 10) 
    {
      break;
    }
  }
  messageA = messageA.substring(0, messageA.length() - 4); 
}

// Draw Greyline
void greyline()
{
  if(greylineRefresh == 1)
  {
    // Draw greyline
    decoded = JpegDec.decodeFsFile("/greyline.jpg");
    if (decoded) {
      M5.Lcd.drawJpgFile(SPIFFS, "/greyline.jpg", 0, 101, 320, 139, 0, 11, JPEG_DIV_2);
    }
    greylineRefresh = 0;
  }
}

// Draw Propag Condition
void propagCondition()
{
  for(uint8_t i = 0; i < 56; i += 1)
  {
    M5.Lcd.drawFastHLine(  0, 44 + i, 320, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.drawFastHLine(  0, 44 + i + 1, 320, TFT_WHITE);
    if (i < 8)
    {
      scrollA(5);
      scrollB(5);
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
  for(uint8_t i = 0; i <= 3; i += 1)
  {
    tmpString = hamQSLData;
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
  for(uint8_t i = 4; i <= 7; i += 1)
  {
    tmpString = hamQSLData;
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

  for(uint16_t i = 0; i <= 500; i += 1)
  {
    if(screenRefresh == 1)
    {
      break;
    }
    delay(10);
  }

  for(uint8_t i = 0; i < 56; i += 1)
  {
    M5.Lcd.drawFastHLine(  0, 99 - i, 320, TFT_BLACK);
    M5.Lcd.drawFastHLine(  0, 99 - i - 1, 320, TFT_WHITE);
    if (i > 48)
    {
      scrollA(5);
      scrollB(5);
    }
    else
    {
      delay(10);
    }
  }
}