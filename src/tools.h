// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

int btnA, btnB, btnC;

// Get Battery level
int8_t getBatteryLevel(bool type)
{
  return M5.Power.getBatteryLevel();
}

// Control if charging
bool isCharging()
{
  return M5.Power.isCharging();
}

// Get button
void getButton()
{
  if (M5.getBoard() == m5::board_t::board_M5ATOM)
  {
    M5.update();
    btnB = M5.BtnA.isPressed();
    btnA = 0;
    btnC = 0;
    if(digitalRead(32) == 0) btnA = 1;
    else if(digitalRead(26) == 0) btnC = 1;
  }
  else
  {
    M5.update();

    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.isPressed();
    btnC = M5.BtnC.isPressed();
  }

  //Serial.printf("%d %d %d\n", btnA, btnB, btnC);
}

// Manage brightness
void setBrightness(uint8_t value)
{
  display.setBrightness(value);
}

// Shutdown
void shutdown()
{
  M5.Power.powerOff();
}