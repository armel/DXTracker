// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

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
  int8_t batteryPercentage = ( baterryVoltage < 3.2 ) ? 0 : ( baterryVoltage - 3.2 ) * 100;

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
}