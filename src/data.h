// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// get Greyline data
void getGreyline()
{
  reloadState = "Greyline";
  if (DEBUG) Serial.println(reloadState);
  
  display.drawJpgUrl(endpointGreyline[maps],  0 + offsetX, 101 + offsetY, 320, 139, 0, 11);
  
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