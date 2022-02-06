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
  if(screenRefresh == 1) {
    M5.Lcd.clear();
    M5.Lcd.fillRect(0, 0, 320, 44, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.drawFastHLine(  0, 0, 320, TFT_WHITE);
    M5.Lcd.drawFastHLine(  0, 44, 320, TFT_WHITE);
    M5.Lcd.drawFastHLine(  0, 100, 320, TFT_WHITE);
  }
}

// Manage message cycle
String binarise()
{
  switch(messageCurrent)
  {
    case 0: return "00"; break;
    case 1: return "01"; break;
    case 2: return "10"; break;
    case 3: return "11"; break;
  }

  return "00";
}

// Get local time
void updateLocalTime()
{
  char timeStringBuff[20];  //20 chars should be enough
  char utcStringBuff[20];   //20 chars should be enough

  struct tm timeinfo;

  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  strftime(timeStringBuff, sizeof(timeStringBuff), "%H:%M:%S %d-%m-%y", &timeinfo);
  strftime(utcStringBuff, sizeof(utcStringBuff), "%z", &timeinfo);

  sscanf(utcStringBuff, "%d", &utc);
  utc = utc / 100;

  //Serial.println(utc);

  dateString = String(timeStringBuff);
}

// Build scroll A
void buildScrollA()
{
  int16_t h = 20;
  int16_t w;

  imgA.setFreeFont(&FreeSans9pt7b); 
  w = imgA.textWidth(messageA) + 80;
  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    imgA.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  imgA.setTextColor(TFT_WHITE); // White text, no background colour
  imgA.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  imgA.drawString(messageA, posA, 2);
  imgA.drawString(messageA, posA - w, 2);
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
    //posA = M5.Lcd.width();
    imgA.setFreeFont(&FreeSans9pt7b); 
    posA = imgA.textWidth(messageA) + 80;
  }

  vTaskDelay(pdMS_TO_TICKS(pause));
}

// Build scroll B
void buildScrollB()
{
  int16_t h = 20;
  int16_t w;

  imgB.setTextSize(1);          // Font size scaling is x1
  imgB.setTextFont(2);          // Font 2 selected
  w = imgB.textWidth(messageB) + 80;
  // We could just use fillSprite(color) but lets be a bit more creative...
  while (h--)
    imgB.drawFastHLine(0, h, w, TFT_BLACK);

  // Now print text on top of the graphics
  imgB.setTextColor(M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b)); // Gray text, no background colour
  imgB.setTextWrap(false);      // Turn of wrap so we can print past end of sprite

  // Need to print twice so text appears to wrap around at left and right edges
  imgB.drawString(messageB, posB, 2);
  imgB.drawString(messageB, posB - w, 2);
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
    //posB = M5.Lcd.width();
    imgB.setTextSize(1);          // Font size scaling is x1
    imgB.setTextFont(2);          // Font 2 selected
    posB = imgB.textWidth(messageB) + 80;
  }

  vTaskDelay(pdMS_TO_TICKS(pause));
}

// Draw title
void title(String title)
{
  static String titleOld;
  static String baselineOld;
  static String reloadStateOld;
  static uint8_t batteryOld;
  static boolean chargingOld;

  if(screenRefresh == 1 || screenRefresh == 2) {
    titleOld = "";
    baselineOld = "";
    reloadStateOld = "";
    batteryOld = 0;
    screenRefresh = 0;
  }

  // Title
  if(title != titleOld) { // Refresh
    titleOld = title;
    reloadStateOld = " ";

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.setFreeFont(&dot15pt7b);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(320);
    M5.Lcd.drawString(title, 160, 16);
  }

  if(alternance % 2 == 0)
  {
    tmpString = "Update at " + dateString;
  }
  else if(alternance == 5)
  {
    tmpString = String(WiFi.localIP().toString().c_str());
  }
  else
  {
    tmpString = String(NAME) + " Version " + String(VERSION);
  }

  if(tmpString != baselineOld) { // Refresh
    baselineOld = tmpString;

    M5.Lcd.setTextColor(TFT_WHITE, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.setFreeFont(0);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.setTextPadding(320);
    M5.Lcd.drawString(tmpString, 160, 36);
  }

  // On right, view reload data
  tmpString = reloadState;
  
  if(tmpString != reloadStateOld) { // Refresh
    reloadStateOld = tmpString;

    if(tmpString != "") {
      M5.Lcd.drawFastHLine(2, 35, 10, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
      M5.Lcd.drawLine(12, 35, 8, 31, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
      M5.Lcd.drawLine(12, 35, 8, 39, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
    }
    else {
      M5.Lcd.drawFastHLine(2, 35, 10, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawLine(12, 35, 8, 31, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.drawLine(12, 35, 8, 39, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    }

    M5.Lcd.setTextColor(M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b), M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    M5.Lcd.setFreeFont(0);
    M5.Lcd.setTextDatum(ML_DATUM);
    M5.Lcd.setTextPadding(60);
    M5.Lcd.drawString(tmpString, 18, 36);
  }

  // On left, view battery level
  uint8_t val = map(getBatteryLevel(1), 0, 100, 0, 16);

  if(val != batteryOld) { // Refresh
    M5.Lcd.drawRect(294, 30, 20, 12, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
    M5.Lcd.drawRect(313, 33, 4, 6, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
    M5.Lcd.fillRect(296, 32, val, 8, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
  }

  // What a dirty code...
  if(isCharging() != chargingOld) {
    chargingOld = isCharging();
    
    M5.Lcd.fillRect(277, 30, 16, 12, M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));

    if(isCharging()) {
      M5.Lcd.setTextColor(M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b), M5.Lcd.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      M5.Lcd.setTextFont(2);
      M5.Lcd.setTextPadding(0);
      M5.Lcd.drawString("D", 280, 36);
      M5.Lcd.drawLine(277, 33, 280, 33, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
      M5.Lcd.drawLine(277, 38, 280, 38, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
      M5.Lcd.drawLine(286, 35, 289, 35, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
      M5.Lcd.drawLine(286, 36, 289, 36, M5.Lcd.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
    }
  }
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
  if(binarise().charAt(1) == '0')
  {
    messageB = "VHF Conditions -- ";

    for (uint8_t i = 0; i < 4; i++)
    {
      // Current propagation 50 MHz
      tmpString = hamQSLData;
      tmpString.replace(skipKey[i], "(");
      tmpString.replace("</phenomenon>", ")");
      parenthesisBegin = tmpString.indexOf("(");
      parenthesisLast = tmpString.indexOf(")", parenthesisBegin);
      if (parenthesisBegin > 0)
      {
        tmpString = tmpString.substring(parenthesisBegin + 1, parenthesisLast);
      }

      tmpString.trim();

      messageB += skipData[i] + " : " + tmpString;
      messageB += " -- ";
    }

    messageB = messageB.substring(0, messageB.length() - 4);
  }
  else if(binarise().charAt(1) == '1')
  {
    messageB = "HF Conditions -- ";

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
      //tmpString.toUpperCase();

      messageB += propagKey[i].substring(0, 7) + " Day " + tmpString + " -- ";
    }

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
      //tmpString.toUpperCase();

      messageB += propagKey[i].substring(0, 7) + " Night " + tmpString + " -- ";
    }
    messageB = messageB.substring(0, messageB.length() - 4);
  }
}

// Draw Cluster Message
void clusterAndSatMessage()
{
  boolean exclude = 0;
  uint8_t counter = 0;
  uint32_t tmp = 0;

  if(binarise().charAt(0) == '0')
  {
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
        if(abs(tmp - frequencyExclude[j]) <= 2 || tmp > 470000)
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
    if(messageA != "")
    {
      messageA = "DX Cluster -- " + messageA;
      messageA = messageA.substring(0, messageA.length() - 4);
    }
    else
    {
      messageA = "DX Cluster -- Data acquisition on the way, please wait...";
    }
  }
  else if(binarise().charAt(0) == '1')
  {
    messageA = "";
    if(satData.length() > 32)
    {
      messageA = satData.substring(15, satData.length() - 3);
    }
    if(messageA != "")
    {
      messageA = "Satellites Passes -- " + messageA;
    }
    else
    {
      messageA = "Satellites Passes -- Data acquisition on the way, please wait...";
    }
  }
}

// Draw Greyline
void greyline()
{  
  if(greylineRefresh == 1)
  {
    // Draw greyline
    decoded = JpegDec.decodeFsFile("/greyline.jpg");
    if (decoded) {
      M5.Lcd.drawJpgFile(SPIFFS, "/greyline.jpg", 0, 101, 320, 139, 0, 11);
      greylineRefresh = 0;
    }
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

    //Serial.println((accY * 1000) < -500);

    if(int(accY * 1000) < -500 && M5.Lcd.getRotation() != 3) {
      M5.Lcd.setRotation(3);
      screenRefresh = 1;
      greylineRefresh = 1;
    }
    else if(int(accY * 1000) > 500 && M5.Lcd.getRotation() != 1) {
      M5.Lcd.setRotation(1);
      screenRefresh = 1;
      greylineRefresh = 1;
    }
  }
}

// Manage scroll
void scroll()
{
  if(screenRefresh == 1)
  {
    return;
  }

  for(uint16_t i = 0; i < 10; i += 1)
  {
    scrollA(5);
    scrollB(5);
  }
}

// Manage screensaver
void wakeAndSleep()
{
  if (screensaverMode == 0 && millis() - screensaver > TIMEOUT_SCREENSAVER)
  {
    for (uint8_t i = brightnessCurrent; i >= 1; i--)
    {
      setBrightness(i);
      scrollA(0);
      scrollB(0);
      delay(10);
    }
    screensaverMode = 1;
    M5.Lcd.sleep();
  }
  else if (screensaverMode == 1 && millis() - screensaver < TIMEOUT_SCREENSAVER)
  {
    M5.Lcd.wakeup();
    screensaverMode = 0;
    for (uint8_t i = 1; i <= brightnessCurrent; i++)
    {
      setBrightness(i);
      scrollA(0);
      scrollB(0);
      delay(10);
    }
  }
}

// M5Screen2bmp (dump the screen to a file)

bool M5Screen2bmp(fs::FS &fs, const char * path){
  uint16_t image_height = M5.Lcd.height();
  uint16_t image_width = M5.Lcd.width();
  const uint16_t pad = (4 - (3 * image_width) % 4) % 4;
  uint16_t filesize = 54 + (3 * image_width + pad) * image_height; 
  unsigned char swap;
  unsigned char line_data[image_width * 3 + pad];
  unsigned char header[54] = { 
    'B','M',  // BMP signature (Windows 3.1x, 95, NT, …)
    0,0,0,0,  // image file size in bytes
    0,0,0,0,  // reserved
    54,0,0,0, // start of pixel array
    40,0,0,0, // info header size
    0,0,0,0,  // image width
    0,0,0,0,  // image height
    1,0,      // number of color planes
    24,0,     // bits per pixel
    0,0,0,0,  // compression
    0,0,0,0,  // image size (can be 0 for uncompressed images)
    0,0,0,0,  // horizontal resolution (dpm)
    0,0,0,0,  // vertical resolution (dpm)
    0,0,0,0,  // colors in color table (0 = none)
    0,0,0,0 };// important color count (0 = all colors are important)
    
  // Open file for writing
  // The existing image file will be replaced
  File file = fs.open(path, FILE_WRITE);

  if(file){
    // fill filesize, width and heigth in the header array
    for(uint8_t i = 0; i < 4; i++) {
        header[ 2 + i] = (char)((filesize>>(8 * i)) & 255);
        header[18 + i] = (char)((image_width  >>(8 * i)) & 255);
        header[22 + i] = (char)((image_height >>(8 * i)) & 255);
    }
    // write the header to the file
    file.write(header, 54);
    
    // initialize padded pixel with 0 
    for(uint16_t i = (image_width - 1) * 3; i < (image_width * 3 + pad); i++){
      line_data[i]=0;
    }
    // The coordinate origin of a BMP image is at the bottom left.
    // Therefore, the image must be read from bottom to top.
    for(uint16_t y = image_height; y > 0; y--){
      // get one line of the screen content
      M5.Lcd.readRectRGB(0, y - 1, image_width, 1, line_data);
      // BMP color order is: Blue, Green, Red
      // return values from readRectRGB is: Red, Green, Blue
      // therefore: R und B need to be swapped
      for(uint16_t x = 0; x < image_width; x++){
        swap = line_data[x * 3];
        line_data[x * 3] = line_data[x * 3 + 2];
        line_data[x * 3 + 2] = swap;
      }
      // write the line to the file
      file.write(line_data, (image_width * 3) + pad);
    }
    file.close();
    return true;
  }
  return false;
}

// M5Screen2bmp (dump the screen to a WiFi client)

bool M5Screen2bmp(){
  uint16_t image_height = M5.Lcd.height();
  uint16_t image_width = M5.Lcd.width();
  const uint16_t pad = (4 - (3 * image_width) % 4) % 4;
  uint16_t filesize = 54 + (3 * image_width + pad) * image_height; 
  unsigned char swap;
  unsigned char line_data[image_width * 3 + pad];
  unsigned char header[54] = { 
    'B','M',  // BMP signature (Windows 3.1x, 95, NT, …)
    0,0,0,0,  // image file size in bytes
    0,0,0,0,  // reserved
    54,0,0,0, // start of pixel array
    40,0,0,0, // info header size
    0,0,0,0,  // image width
    0,0,0,0,  // image height
    1,0,      // number of color planes
    24,0,     // bits per pixel
    0,0,0,0,  // compression
    0,0,0,0,  // image size (can be 0 for uncompressed images)
    0,0,0,0,  // horizontal resolution (dpm)
    0,0,0,0,  // vertical resolution (dpm)
    0,0,0,0,  // colors in color table (0 = none)
    0,0,0,0 };// important color count (0 = all colors are important)

  // fill filesize, width and heigth in the header array
  for(uint8_t i = 0; i < 4; i++) {
      header[ 2 + i] = (char)((filesize>>(8 * i)) & 255);
      header[18 + i] = (char)((image_width  >> (8 * i)) & 255);
      header[22 + i] = (char)((image_height >> (8 * i)) & 255);
  }
  // write the header to the file
  httpClient.write(header, 54);
  
  // To keep the required memory low, the image is captured line by line
  // initialize padded pixel with 0 
  for(uint16_t i = (image_width - 1) * 3; i < (image_width * 3 + pad); i++){
    line_data[i]=0;
  }
  // The coordinate origin of a BMP image is at the bottom left.
  // Therefore, the image must be read from bottom to top.
  for(uint16_t y = image_height; y > 0; y--){
    // get one line of the screen content
    M5.Lcd.readRectRGB(0, y - 1, image_width, 1, line_data);
    // BMP color order is: Blue, Green, Red
    // return values from readRectRGB is: Red, Green, Blue
    // therefore: R und B need to be swapped
    for(uint16_t x = 0; x < image_width; x++){
      swap = line_data[x * 3];
      line_data[x * 3] = line_data[x * 3 + 2];
      line_data[x * 3 + 2] = swap;
    }
    // write the line to the file
    httpClient.write(line_data, (image_width * 3) + pad);
  }
  return true;
}

// Get screenshot

void getScreenshot()
{
  unsigned long timeout_millis = millis() + 2000;
  String currentLine = "";                

  httpClient = httpServer.available(); 

  if(WiFi.status() == WL_CONNECTED){
    //client.setNoDelay(1);
    if (httpClient) {  
      // Force a disconnect after 2 seconds
      Serial.println("New Client.");  
      // Loop while the client's connected
      while (httpClient.connected()) { 
        // If the client is still connected after 2 seconds,
        // Something is wrong. So kill the connection
        if(millis() > timeout_millis){
          Serial.println("Force Client stop!");  
          httpClient.stop();
        } 
        // If there's bytes to read from the client,
        if (httpClient.available()) {             
          char c = httpClient.read();            
          Serial.write(c);    
          // If the byte is a newline character             
          if (c == '\n') {    
            // Uwo newline characters in a row (empty line) are indicating
            // The end of the client HTTP request, so send a response:
            if (currentLine.length() == 0) {
              // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
              // and a content-type so the client knows what's coming, then a blank line,
              // followed by the content:

              screensaver = millis(); // Screensaver update !!!

              switch (htmlGetRequest)
              {
                case GET_index_page: {
                  httpClient.println("HTTP/1.1 200 OK");
                  httpClient.println("Content-type:text/html");
                  httpClient.println();
                  httpClient.write_P(index_html, sizeof(index_html));
                  break;
                }
                case GET_screenshot: {              
                  httpClient.println("HTTP/1.1 200 OK");
                  httpClient.println("Content-type:image/bmp");
                  httpClient.println();
                  M5Screen2bmp();
                  break;
                }
                default:
                  httpClient.println("HTTP/1.1 404 Not Found");
                  httpClient.println("Content-type:text/html");
                  httpClient.println();
                  httpClient.print("404 Page not found.<br>");
                  break;
              }
              // The HTTP response ends with another blank line:
              httpClient.println();
              // Break out of the while loop:
              break;
            } else {    // if a newline is found
              // Analyze the currentLine:
              // detect the specific GET requests:
              if(currentLine.startsWith("GET /")){
                htmlGetRequest = GET_unknown;
                // If no specific target is requested
                if(currentLine.startsWith("GET / ")){
                  htmlGetRequest = GET_index_page;
                }
                // If the screenshot image is requested
                if(currentLine.startsWith("GET /screenshot.bmp")){
                  htmlGetRequest = GET_screenshot;
                }
                // If the button left was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonLeft")){
                  buttonLeftPressed = true;
                  htmlGetRequest = GET_index_page;
                }
                // If the button center was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonCenter")){
                  buttonCenterPressed = true;
                  htmlGetRequest = GET_index_page;
                }
                // If the button right was pressed on the HTML page
                if(currentLine.startsWith("GET /buttonRight")){
                  buttonRightPressed = true;
                  htmlGetRequest = GET_index_page;
                }
              }
              currentLine = "";
            }
          } else if (c != '\r') {  
            // Add anything else than a carriage return
            // character to the currentLine 
            currentLine += c;      
          }
        }
      }
      // Close the connection
      httpClient.stop();
      Serial.println("Client Disconnected.");
    }
  }
}

// List all files on SPIFFS
void listAllFiles(){
  File root = SPIFFS.open("/"); 
  File file = root.openNextFile();
 
  while(file){
      Serial.print("FILE: ");
      Serial.println(file.name());
      file = root.openNextFile();
  }
}

// List all files on SPIFFS
void getBinaryList(File dir)
{
  while (true) {
    File entry =  dir.openNextFile();
    if (!entry) {
      // no more files
      break;
    }

    if(strstr(entry.name(), "/.") == NULL && strstr(entry.name(), ".bin") != NULL) {
      binFilename[binIndex] = entry.name();
      binIndex++;
    }

    if (entry.isDirectory() && strstr(entry.name(), "/.") == NULL) {
      getBinaryList(entry);
    }

    entry.close();
  }
}

// Bin Loader
void binLoader() {
  boolean click = 0;
  int8_t cursor = 0;
  int8_t change = 255;
  String tmpName;

  if(!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");

    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);

    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.drawString("Flash File System", 160, 20);
    M5.Lcd.drawString("needs to be formated.", 160, 50);
    M5.Lcd.drawString("It takes around 4 minutes.", 160, 100);
    M5.Lcd.drawString("Please, wait until ", 160, 150);
    M5.Lcd.drawString("the application starts !", 160, 180);

    Serial.println("SPIFFS Formating...");

    SPIFFS.format();    // Format SPIFFS...

    M5.Lcd.setTextFont(0);
    M5.Lcd.setTextSize(0);

    return;
  }
  
  root = SPIFFS.open("/");
  getBinaryList(root);

  if(binIndex != 0) {
    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(1);

    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);

    for (uint8_t i = TIMEOUT_BIN_LOADER * 10; i > 0; i--) {
      getButton();

      if( i % 10 == 0) {
        tmpName += ".";
        M5.Lcd.drawString(tmpName, 160, 20);
      }

      if(btnA || btnC) {
        return;
      }
      else if(btnB) {
        click = 1;
        break;
      }

      vTaskDelay(100);
    }
  }

  while(click == 1) {
    while(btnB != 0) {
      getButton();
      vTaskDelay(100);
    }

    M5.Lcd.setTextFont(1);
    M5.Lcd.setTextSize(2);

    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.setTextDatum(CC_DATUM);
    M5.Lcd.drawString("Bin Loader", 160, 20);

    getButton();

    if(btnA) {
      cursor--;
    }
    else if(btnC) {
      cursor++;
    }
    else if(btnB) {
      updateFromFS(SPIFFS, binFilename[cursor]);
      ESP.restart(); 
    }

    cursor = (cursor < 0) ? binIndex - 1 : cursor;
    cursor = (cursor > binIndex - 1) ? 0 : cursor;

    if(change != cursor) {
      change = cursor;
      M5.Lcd.setTextPadding(320);

      for(uint8_t i = 0; i < binIndex; i++) {
        tmpName = binFilename[i].substring(1);

        if (cursor == i) {
          tmpName = ">> " + tmpName + " <<";
        }
        
        M5.Lcd.drawString(tmpName, 160, 60 + i * 20);
      }
    }
    vTaskDelay(100);
  }
}