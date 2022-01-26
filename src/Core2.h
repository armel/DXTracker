// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

CORE2SPK _Core2SPK;

int btnA, btnB, btnC;

// Expand virtual fucking button
Button myBtnA(  0, 200, 100,  80);
Button myBtnB(110, 200, 100,  80);
Button myBtnC(220, 200, 100,  80);

// Power init
void power()
{
  //By default, M5.begin() will initialize AXP192 chip on Core2
  M5.Axp.SetLed(0);
}

// Get Battery level
int8_t getBatteryLevel(bool type)
{
  float baterryVoltage = M5.Axp.GetBatVoltage();
  int16_t batteryPercentage = ( baterryVoltage < 3.2 ) ? 0 : ( baterryVoltage - 3.2 ) * 100;

  if(batteryPercentage > 100) {
    batteryPercentage = 100;
  }

  if(type == 0) {
    if(batteryPercentage > 80)
    {
        return 100;
    }
    else if (batteryPercentage > 60)
    {
        return 75;
    }
    else if (batteryPercentage > 40)
    {
        return 50;
    }
    else if (batteryPercentage > 20)
    {
        return 25;
    }
    return 0;
  }
  else 
  {
    return batteryPercentage;
  }
}

// Control if charging
bool isCharging()
{
  return M5.Axp.isCharging();
}

// Get button
void getButton()
{
  M5.update();
  btnA = myBtnA.read();
  btnB = myBtnB.read();
  btnC = myBtnC.read();

  if(btnA || btnB || btnC) 
  {
    _Core2SPK.playBeep(5000, 100, 2000, false);
  }
}

// Manage Vibrator
void vibration(int t) {
  M5.Axp.SetLDOEnable(3, true);
  delay(t);
  M5.Axp.SetLDOEnable(3, false);
}

// Speaker
void speaker()
{
  M5.Axp.SetSpkEnable(true);
  _Core2SPK.begin();
  _Core2SPK.playBeep(5000, 100, 2000, false);
  vibration(200);
}

// Manage Tone
void totTone()
{
  _Core2SPK.playBeep(5000, 100, 2000, false);
  vTaskDelay(pdMS_TO_TICKS(100));
  //M5.Axp.SetLed(1);
}

// Manage Mute
void totMute()
{
  //M5.Axp.SetLed(0);
}

// Manage brightness
void setBrightness(uint8_t value)
{
  uint16_t tmp = map(value, 0, 128, 2500, 3300);
  M5.Axp.SetLcdVoltage(tmp); // from 2500 to 3300
}