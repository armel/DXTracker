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
    display.clear();
    display.fillRect(0, 0, 320, 44, display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    display.drawFastHLine(  0, 0, 320, TFT_WHITE);
    display.drawFastHLine(  0, 44, 320, TFT_WHITE);
    display.drawFastHLine(  0, 100, 320, TFT_WHITE);
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

  imgA.setFont(&FreeSans9pt7b); 
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

  posA -= 2;
  if (posA < 0)
  {
    //posA = display.width();
    imgA.setFont(&FreeSans9pt7b); 
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
  imgB.setTextColor(display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b)); // Gray text, no background colour
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
  if (posB < 0)
  {
    //posB = display.width();
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

  if(screenRefresh == 1 || screenRefresh == 2) {
    titleOld = "";
    baselineOld = "";
    reloadStateOld = "";
    screenRefresh = 0;
  }

  // Title
  if(title != titleOld) { // Refresh
    titleOld = title;
    reloadStateOld = " ";

    display.setTextColor(TFT_WHITE, display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    display.setFont(&dot15pt7b);
    display.setTextDatum(CC_DATUM);
    display.setTextPadding(320);
    display.drawString(title, 160, 16);
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

    display.setTextColor(TFT_WHITE, display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    display.setFont(0);
    display.setTextDatum(CC_DATUM);
    display.setTextPadding(320);
    display.drawString(tmpString, 160, 36);
  }

  // On right, view reload data
  tmpString = reloadState;
  
  if(tmpString != reloadStateOld) { // Refresh
    reloadStateOld = tmpString;

    if(tmpString != "") {
      display.drawFastHLine(2, 35, 10, display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
      display.drawLine(12, 35, 8, 31, display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
      display.drawLine(12, 35, 8, 39, display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
    }
    else {
      display.drawFastHLine(2, 35, 10, display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      display.drawLine(12, 35, 8, 31, display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
      display.drawLine(12, 35, 8, 39, display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    }

    display.setTextColor(display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b), display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    display.setFont(0);
    display.setTextDatum(ML_DATUM);
    display.setTextPadding(60);
    display.drawString(tmpString, 18, 36);
  }

  // On left, view battery level
  uint8_t val = map(getBatteryLevel(1), 0, 100, 0, 16);

  display.drawRect(294, 30, 20, 12, display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
  display.drawRect(313, 33, 4, 6, display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
  display.fillRect(296, 32, val, 8, display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b));
    
  if(isCharging()) {
    display.setTextColor(display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b), display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    display.setFont(0);
    display.setTextDatum(CC_DATUM);
    display.setTextPadding(0);
    display.drawString("+", 288, 37);
  }
  else {
    display.setTextColor(display.color565(TFT_GRAY.r, TFT_GRAY.g, TFT_GRAY.b), display.color565(TFT_BACK.r, TFT_BACK.g, TFT_BACK.b));
    display.setFont(0);
    display.setTextDatum(CC_DATUM);
    display.setTextPadding(0);
    display.drawString(" ", 288, 37);
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
  uint8_t next = 0;
  uint8_t counter = 0;
  static uint8_t messageOld = 64;
  long tmp = 0;

  if(messageOld != messageCurrent && reloadState == "") {
    messageOld = messageCurrent;

    if(binarise().charAt(0) == '0')
    {
      posA = 80;
      messageA = "";

      size_t n = sizeof(frequencyExclude)/sizeof(frequencyExclude[0]);

      cluster = getValue(hamQTHData, '|', next);
      while(cluster != "")
      {
        frequency = getValue(cluster, '^', 1);
        tmp = frequency.toInt();

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
          messageA += getValue(cluster, '^', 0) + " " + getValue(cluster, '^', 8) + " " + frequency + " " + getValue(cluster, '^', 9) + " -- ";
          counter += 1;
        }

        if(counter == 20) 
        {
          break;
        }

        next++;
        cluster = getValue(hamQTHData, '|', next);
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
    else if(binarise().charAt(0) == '1' && reloadState == "")
    {
      posA = 80;
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
    for (uint8_t i = brightnessCurrent; i >= 1; i -= 4)
    {
      setBrightness(i);
      scrollA(0);
      scrollB(0);
      delay(10);
    }

    screensaverMode = 1;
    display.sleep();
    screensaver = millis();
  }
  else if (screensaverMode == 1 && millis() - screensaver > TIMEOUT_SCREENSAVER)
  {
    screensaver = millis();
    display.wakeup();
    screensaverMode = 0;

    for (uint8_t i = 1; i <= brightnessCurrent; i += 4)
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
  uint16_t image_height = display.height();
  uint16_t image_width = display.width();
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
      display.readRectRGB(0, y - 1, image_width, 1, line_data);
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
  uint16_t image_height = display.height();
  uint16_t image_width = display.width();
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
    display.readRectRGB(0, y - 1, image_width, 1, line_data);
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
                  if(M5.getBoard() == m5::board_t::board_M5Stack) {
                    httpClient.write_P(index_m5stack_html, sizeof(index_m5stack_html));
                  }
                  else if(M5.getBoard() == m5::board_t::board_M5StackCore2) {
                    httpClient.write_P(index_core2_html, sizeof(index_core2_html));
                  }
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

// List files on SPIFFS or SD
void getBinaryList(File dir, String type)
{
  while (true)
  {
    File entry = dir.openNextFile();
    if (!entry)
    {
      // no more files
      break;
    }

    if (strstr(entry.name(), "/.") == NULL && strstr(entry.name(), ".bin") != NULL)
    { 
      //Serial.println(type + "_" + entry.name());     
      binFilename[binIndex] = type + "_" + entry.name();
      binIndex++;
    }

    if (entry.isDirectory() && strstr(entry.name(), "/.") == NULL)
    {
      getBinaryList(entry, type);
    }

    entry.close();
  }
}

// Bin Loader
void binLoader()
{
  boolean click = 0;
  int8_t cursor = 0;
  int8_t start = 0;
  int8_t stop = 0;
  int8_t limit = 8;
  int8_t change = 255;
  String tmpName;

  if (!SPIFFS.begin())
  {
    Serial.println("SPIFFS Mount Failed");

    display.setTextFont(1);
    display.setTextSize(2);

    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextDatum(CC_DATUM);
    display.drawString("Flash File System", 160, 20);
    display.drawString("needs to be formated.", 160, 50);
    display.drawString("It takes around 4 minutes.", 160, 100);
    display.drawString("Please, wait until ", 160, 150);
    display.drawString("the application starts !", 160, 180);

    Serial.println("SPIFFS Formating...");

    SPIFFS.format(); // Format SPIFFS...

    display.setTextFont(0);
    display.setTextSize(0);

    return;
  }

  root = SPIFFS.open("/");
  getBinaryList(root, "SP");

  if (SD.begin(GPIO_NUM_4, SPI, 25000000)) 
  {
    root = SD.open("/");
    getBinaryList(root, "SD");
  }

  if (binIndex != 0)
  {
    // QRCode
    display.qrcode("https://github.com/armel/ICSMeter", 90, 80, 140, 6);

    display.setTextFont(1);
    display.setTextSize(1);

    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextDatum(CC_DATUM);

    for (uint8_t i = TIMEOUT_BIN_LOADER * 10; i > 0; i--)
    {
      getButton();

      if (i % 10 == 0)
      {
        tmpName += ".";
        display.drawString(tmpName, 160, 20);
      }

      if (btnA || btnC)
      {
        SD.end(); // If not Bluetooth doesn't work !!!
        return;
      }
      else if (btnB)
      {
        click = 1;
        display.fillRect(0, 0, 320, 240, TFT_BLACK);
        break;
      }

      vTaskDelay(100);
    }
  }

  while (click == 1)
  {
    while (btnB != 0)
    {
      getButton();
      vTaskDelay(100);
    }

    display.setTextFont(1);
    display.setTextSize(2);

    display.setTextColor(TFT_WHITE, TFT_BLACK);
    display.setTextDatum(CC_DATUM);
    display.drawString("Bin Loader V0.2", 160, 20);

    getButton();

    if (btnA)
    {
      cursor--;
    }
    else if (btnC)
    {
      cursor++;
    }
    else if (btnB)
    {
      if(binFilename[cursor].substring(0, 4) == "SP_/") {
        updateFromFS(SPIFFS, binFilename[cursor].substring(3));
      }
      else {
        updateFromFS(SD, binFilename[cursor].substring(3));
      }
      ESP.restart();
    }

    cursor = (cursor < 0) ? binIndex - 1 : cursor;
    cursor = (cursor > binIndex - 1) ? 0 : cursor;

    start = cursor / limit;

    stop = (start * limit) + limit;
    
    /*
    Serial.print(cursor);
    Serial.print("-");
    Serial.print(start);
    Serial.print("-");
    Serial.print(stop);
    Serial.println("----------");
    */

    if (change != cursor)
    {
      change = cursor;
      display.setTextPadding(320);

      uint8_t i = 0;
      for (uint8_t j = (start * limit); j < stop; j++)
      {
        tmpName = binFilename[j].substring(4);

        if (cursor == j)
        {
          tmpName = ">> " + tmpName + " <<";

          if(binFilename[cursor].substring(0, 4) == "SP_/") {
            display.setTextSize(1);
            display.drawString("SPI Flash File Storage", 160, 50);
          }
          else {
            display.setTextSize(1);
            display.drawString("SD Card Storage", 160, 50);
          }
        }

        display.setTextSize(2);
        display.drawString(tmpName, 160, 80 + i * 20);
        i++;
      }
    }
    vTaskDelay(100);
  }
  SD.end(); // If not Bluetooth doesn't work !!!
}

// get Greyline data
void getGreyline()
{
  reloadState = "Greyline";
  if (DEBUG) Serial.println(reloadState);
  
  display.drawJpgUrl(endpointGreyline[greylineSelect],  0, 101, 320, 139, 0, 11);
  
  vTaskDelay(pdMS_TO_TICKS(20));
  reloadState = "";
}

// get Solar data
void getHamQSL()
{
  uint16_t httpCode;

  reloadState = "Solar";
  if (DEBUG) Serial.println(reloadState);

  http.begin(client, endpointHamQSL);       // Specify the URL
  http.addHeader("Content-Type", "text/plain");   // Specify content-type header
  http.setTimeout(1000);                           // Set Time Out
  httpCode = http.GET();                          // Make the request
  if (httpCode == 200)                            // Check for the returning code
  {
    String tmpString = http.getString(); // Get data
    tmpString.trim();

    if(tmpString != "")
    {
      hamQSLData = tmpString;
    }
  }
  http.end(); // Free the resources
  client.flush();
  client.stop();

  vTaskDelay(pdMS_TO_TICKS(20));
  reloadState = "";
}

// get Cluster data
void getHamQTH()
{
  uint16_t httpCode;

  reloadState = "Cluster";
  if (DEBUG) Serial.println(reloadState);

  http.begin(client, endpointHamQTH);       // Specify the URL
  http.addHeader("Content-Type", "text/plain");   // Specify content-type header
  http.setTimeout(1000);                           // Set Time Out
  httpCode = http.GET();                          // Make the request
  if (httpCode == 200)                            // Check for the returning code
  {
    String tmpString = http.getString(); // Get data
    tmpString.trim();
    tmpString.replace("\n", "|");

    if(tmpString != "" && tmpString != hamQTHData)
    {
      hamQTHData = tmpString;
    }
  }
  http.end(); // Free the resources
  client.flush();
  client.stop();

  vTaskDelay(pdMS_TO_TICKS(20));
  reloadState = "";
}

// get Sat data
void getHamSat()
{
  uint16_t httpCode;

  reloadState = "Sat";
  if (DEBUG) Serial.println(reloadState);

  http.begin(client, endpointSat + "?lat=" + config[(configCurrent * 4) + 2] + "&lng=" + config[(configCurrent * 4) + 3] + "&format=text");       // Specify the URL
  http.addHeader("Content-Type", "text/plain");   // Specify content-type header
  http.setTimeout(1000);                          // Set Time Out
  httpCode = http.GET();                          // Make the request
  if (httpCode == 200)                            // Check for the returning code
  {
    String tmpString = http.getString(); // Get data
    tmpString.trim();

    if(tmpString != "" && tmpString != satData)
    {
      satData = tmpString;
    }
  }
  http.end(); // Free the resources
  client.flush();
  client.stop();

  vTaskDelay(pdMS_TO_TICKS(20));
  reloadState = "";
}