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

// Build scroll V
void buildScrollV()
{
  int16_t h = 20;
  int16_t w = M5.Lcd.width() * 8;

  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    imgV.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  imgV.setTextSize(1);          // Font size scaling is x1
  imgV.setTextFont(2);          // Font 2 selected
  //img.setFreeFont(&tahoma6pt7b);
  
  imgV.setTextColor(TFT_WHITE); // White text, no background colour
  imgV.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  imgV.setCursor(posV, 2); // Print text at pos
  imgV.print(messageV);

  imgV.setCursor(posV - w, 2); // Print text at pos - sprite width
  imgV.print(messageV);
}

// Scroll V
void scrollV(uint8_t pause)
{
  // Sprite for scroll
  buildScrollV();
  imgV.pushSprite(0, 52);

  posV -= 1;
  if (posV == 0)
  {
    posV = M5.Lcd.width() * 8;
  }

  delay(pause);
}

// Build scroll H
void buildScrollH()
{
  int16_t h = 20;
  int16_t w = M5.Lcd.width() * 2;

  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    imgH.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  imgH.setTextSize(1);          // Font size scaling is x1
  imgH.setTextFont(2);          // Font 2 selected
  //img.setFreeFont(&tahoma6pt7b);
  
  imgH.setTextColor(M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b)); // White text, no background colour
  imgH.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  imgH.setCursor(posH, 2); // Print text at pos
  imgH.print(messageH);

  imgH.setCursor(posH - w, 2); // Print text at pos - sprite width
  imgH.print(messageH);
}

// Scroll H
void scrollH(uint8_t pause)
{
  // Sprite for scroll
  buildScrollH();
  imgH.pushSprite(0, 74);

  posH -= 1;
  if (posH == 0)
  {
    posH = M5.Lcd.width() * 2;
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
    scrollH(5);
    scrollV(5);
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

  // Update date and time
  tmpString = HamQSLData;
  tmpString.replace("<updated>", "(");
  tmpString.replace("</updated>", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")");
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  M5.Lcd.drawString(tmpString, 160, 36);
}

// Draw Propag Data
void propagData()
{
  // Title
  solarData[alternance].toUpperCase();

  // Current value
  tmpString = HamQSLData;
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

  M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
  M5.Lcd.setFreeFont(&rounded_led_board10pt7b);
  M5.Lcd.setTextDatum(CC_DATUM);
  M5.Lcd.setTextPadding(320);
}

// Draw Propag Message
void propagMessage()
{
  messageH = "";
  // Current propagation 50 MHz
  tmpString = HamQSLData;
  tmpString.replace("location=\"europe\">", "(");
  tmpString.replace("</phenomenon>", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  messageH += "E-Skip Europe " + tmpString;

  messageH += " -- ";

  // Current propagation 50 MHz
  tmpString = HamQSLData;
  tmpString.replace("location=\"europe_6m\">", "(");
  tmpString.replace("</phenomenon>", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  messageH += "E-Skip Europe 6m " + tmpString;

  messageH += " -- ";

  // Current propagation 70 MHz
  tmpString = HamQSLData;
  tmpString.replace("location=\"europe_4m\">", "(");
  tmpString.replace("</phenomenon>", ")");
  parenthesisBegin = tmpString.indexOf("(");
  parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
  if (parenthesisBegin > 0)
  {
    tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
  }

  tmpString.trim();

  messageH += "E-Skip Europe 4m " + tmpString;
}

// Draw Cluster Message
void clusterMessage()
{
  boolean exclude = 0;
  uint8_t counter = 0;
  uint32_t tmp = 0;

  size_t n = sizeof(frequencyExclude)/sizeof(frequencyExclude[0]);

  messageV = "";
  HamQTHData.replace("\n", "|");

  for (uint8_t i = 0; i < 50; i++)
  {
    cluster[i] = getValue(HamQTHData, '|', i);
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

      messageV += call[i] + " " + band[i] + " " + frequency[i] + " " + country[i] + " -- ";
      counter += 1;
    }

    if(counter == 10) 
    {
      break;
    }
  }
  messageV = messageV.substring(0, messageV.length() - 4); 
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
      scrollH(5);
      scrollV(5);
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
    tmpString = HamQSLData;
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
    tmpString = HamQSLData;
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
      scrollH(5);
      scrollV(5);
    }
    else
    {
      delay(10);
    }
  }
}