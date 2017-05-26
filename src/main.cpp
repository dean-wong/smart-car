/*
   功能：利用SR04超声波传感器进行测距，并用串口显示测出的距离值
 */
#include "application.h"
#include <Arduino.h>

Application& app = Application::GetApp();

void setup()
{
    app.Init();
}

void loop()
{
    app.Run();
}
