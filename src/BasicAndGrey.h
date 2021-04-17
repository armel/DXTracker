// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

int btnA, btnB, btnC = 0;

// Power init
void power()
{
    M5.Power.begin();
}

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
    M5.update();
    btnA = M5.BtnA.read();
    btnB = M5.BtnB.read();
    btnC = M5.BtnC.read();
}

// Speaker
void speaker()
{
    M5.Speaker.setVolume(1);
    M5.Speaker.update();
}

// Manage Tone
void totTone()
{
    M5.Speaker.tone(2000);
}

// Manage Mute
void totMute()
{
    M5.Speaker.mute();
}