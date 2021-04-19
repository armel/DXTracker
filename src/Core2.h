// Copyright (c) F4HWN Armel. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

int btnA, btnB, btnC, btnD, btnE = 0;

TouchZone left(0, 0, 160, 240);
TouchZone right(160, 0, 160, 240);
TouchZone top(0, 0, 320, 120);
TouchZone bottom(0, 120, 320, 240);

Gesture swipeLeft(right, left, "Swipe left");
Gesture swipeRight(left, right, "Swipe right");
Gesture swipeBottom(top, bottom, "Swipe down");
Gesture swipeTop(bottom, top, "Swipe top");

// Swipe left manage
void detectSwipeLeft(TouchEvent& e) {
    btnD = -10;
}

// Swipe right manage
void detectSwipeRight(TouchEvent& e) {
    btnD = 10;
}

// Swipe bottom manage
void detectSwipeBottom(TouchEvent& e) {
    btnE = -10;
}

// Swipe top manage
void detectSwipeTop(TouchEvent& e) {
    btnE = 10;
}

// Power init
void power()
{
    //By default, M5.begin() will initialize AXP192 chip on Core2
    M5.Axp.SetLed(0);
    swipeLeft.addHandler(detectSwipeLeft);
    swipeRight.addHandler(detectSwipeRight);
    swipeBottom.addHandler(detectSwipeBottom);
    swipeTop.addHandler(detectSwipeTop);
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
    btnA = M5.BtnA.isPressed();
    btnB = M5.BtnB.isPressed();
    btnC = M5.BtnC.isPressed();

    if(btnD < 0) {
        btnA = 1;
        btnD = 0;
    }
    else if(btnD > 0) {
        btnC = 1;
        btnD = 0;
    }

    if(btnE < 0 || btnE > 0) {
        btnB = 1;
        btnE = 0;
    }
}