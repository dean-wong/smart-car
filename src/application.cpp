/*
 *
 */

#include "application.h"
#include <Arduino.h>

#undef DEBUG
// #define DEBUG

// 设定SR04连接的Arduino引脚
#define SR_TRIG 4
#define SR_ECHO 5

#define LED_INDICATE 13

#define IRPIN_RIGHT 2
#define IRPIN_LEFT 3

#define PIN_ENA 10
#define PIN_IN1 11
#define PIN_IN2 8

#define PIN_IN3 6
#define PIN_IN4 7
#define PIN_ENB 9

Application::Application()
    : ultrasonicSensor(SR_TRIG, SR_ECHO) // ST_HW_HC_SR04(TRIG, ECHO)
    , isObstacleOnLeft(0)
    , isObstacleOnRight(0)
{
}

Application::~Application()
{
}

Application& Application::GetApp()
{
    static Application s_app;
    return s_app;
}

void Application::Log(const char* str) const
{
#ifdef DEBUG
    Serial.print(str);
#endif
}

void Application::onInterrupt()
{
    GetApp().detectObstacle();
}

bool Application::Init()
{

#ifdef DEBUG
    // 初始化串口通信
    Serial.begin(9600);
    Serial.println("Ultra Driver Init...");
#endif

    pinMode(LED_INDICATE, OUTPUT); // 指示灯引脚

    // 红外避障的输入设置，利用中断模式
    pinMode(IRPIN_LEFT, INPUT_PULLUP);
    pinMode(IRPIN_RIGHT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(IRPIN_LEFT), onInterrupt, CHANGE);
    attachInterrupt(digitalPinToInterrupt(IRPIN_RIGHT), onInterrupt, CHANGE);

    // 两个马达的pwm是9和10，调低它们的频率，避免哨叫
    TCCR1B = (TCCR1B & 0b11111000) | 4;
    miniDriver.SetupWheel(Driver::LEFT, PIN_IN4, PIN_IN3, PIN_ENB);
    miniDriver.SetupWheel(Driver::RIGHT, PIN_IN1, PIN_IN2, PIN_ENA);

    // 产生随机种子
    randomSeed(analogRead(0));

    return true;
}

void Application::Run()
{

    if (isObstacleOnLeft && isObstacleOnRight) {
        miniDriver.turnLeft(180);
    } else if (isObstacleOnLeft) {
        miniDriver.turnRight(random(0, 45));
    } else if (isObstacleOnRight) {
        miniDriver.turnLeft(random(0, 45));
    } else {
        double distance = ultrasonicSensor.getDistance();

        if (distance > 0) {
            digitalWrite(LED_INDICATE, HIGH);
#ifdef DEBUG
            String msg = String(distance) + " cm\n";
            Log(msg.c_str());
#endif
            if (distance > 30) {
                miniDriver.forward();
            } else {
                miniDriver.stop(); // 随机摆动 0 - 45之间的角度，便于找到目标
                randomWobble(0, 90);
            }
        } else {
            digitalWrite(LED_INDICATE, LOW);
            miniDriver.stop();
            randomWobble(0, 60);
        }
    }
}

void Application::randomWobble(int start /*= 0*/, int end /*= 90*/)
{
    long randNumber = random(start, end);
    if (randNumber < end / 2) {
        miniDriver.turnLeft(end / 2 - randNumber);
    } else {
        miniDriver.turnRight(end - randNumber);
    }
}

void Application::detectObstacle()
{
    int Left = !digitalRead(IRPIN_LEFT);
    int Right = !digitalRead(IRPIN_RIGHT);
    if (isObstacleOnLeft != Left) {
        isObstacleOnLeft = Left;
    }

    if (isObstacleOnRight != Right) {
        isObstacleOnRight = Right;
    }
}
