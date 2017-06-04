/*
 *
 */

#include "application.h"
#include <Arduino.h>

#undef _DEBUG
// #define _DEBUG
#ifdef _DEBUG
    #define LOG(x)  Log2Serial(x)
#else
    #define LOG(x)  {}
#endif

// 设定SR04连接的Arduino引脚
#define SR_TRIG 4
#define SR_ECHO 5

#define LED_INDICATE 13

// 红外障碍物检测传感器，由于使用了中断，所以必须是2，3引脚
#define IRPIN_RIGHT 2
#define IRPIN_LEFT 3

// 控制马达的引脚，其中9、10用于调整速度，因为这两个引脚的频率相同
#define PIN_ENA 10
#define PIN_IN1 11
#define PIN_IN2 8

#define PIN_ENB 9
#define PIN_IN3 6
#define PIN_IN4 7

// 红外接收器 VS1838B 引脚
#define RECV_PIN 12

// 遥控器代码表
#define  UNKNOWN    -1
#define  CH1        0xFFA25D
#define  CH         0xFF629D
#define  CH2        0xFFE21D
#define  PREV       0xFF22DD
#define  NEXT       0xFF02FD
#define  PLAYPAUSE  0xFFC23D
#define  VOL1       0xFFE01F
#define  VOL2       0xFFA857
#define  EQ         0xFF906F
#define  BUTON0     0xFF6897
#define  BUTON100   0xFF9867
#define  BUTON200   0xFFB04F
#define  BUTON1     0xFF30CF
#define  BUTON2     0xFF18E7
#define  BUTON3     0xFF7A85
#define  BUTON4     0xFF10EF
#define  BUTON5     0xFF38C7
#define  BUTON6     0xFF5AA5
#define  BUTON7     0xFF42BD
#define  BUTON8     0xFF4AB5
#define  BUTON9     0xFF52AD


///////////////////////////////////////////////////////////////////////////////
Application::Application()
    : ultrasonicSensor(SR_TRIG, SR_ECHO) // ST_HW_HC_SR04(TRIG, ECHO)
    , isObstacleOnLeft(0)
    , isObstacleOnRight(0)
    , irRecv(RECV_PIN)
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

void Application::Log2Serial(const char* str) const
{
#ifdef _DEBUG
    Serial.print(str);
#endif
}

void Application::onInterrupt()
{
    GetApp().detectObstacle();
}

bool Application::Init()
{

#ifdef _DEBUG
    // 初始化串口通信
    Serial.begin(9600);
    Log2Serial("Ultra Driver Init...");
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

    LOG("Enabling IRin");
    irRecv.enableIRIn();

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

        const bool isMoving = miniDriver.isMoving();
        const MOVEMENT action = (MOVEMENT)readIRCode();
        const double distance = ultrasonicSensor.getDistance();

        do{
            // 紧急停车
            if (0 < distance && distance < 30){
                LOG("Immediately stop!!!");
                // miniDriver.stop(); 
                randomWobble(0, 90);
                break;
            }

            switch(action){
                case LEFT: 
                    LOG("Turn left.");
                    miniDriver.turnLeft(25); 
                    break;
                case RIGHT: 
                    LOG("Turn right.");
                    miniDriver.turnRight(25); 
                    break;
                case MOVE:
                    LOG("=== Action ===");
                    isMoving?miniDriver.stop():miniDriver.forward();
                    break;
                default: 
                    break;
            }

        }while(false);

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

int Application::readIRCode()
{
    MOVEMENT r = UNDEFINE;
    decode_results results;
    if (irRecv.decode(&results)) { 
        switch(results.value){
            case UNKNOWN:   r = UNDEFINE; break;
            case CH1:       r = LEFT;    break;
            case CH:        r = RIGHT;   break;
            case CH2:       r = MOVE;    break;
            case PREV:      r = LEFT;    break;
            case NEXT:      r = RIGHT;   break;
            case PLAYPAUSE: r = MOVE;    break;
            case VOL1:      r = LEFT;    break;
            case VOL2:      r = RIGHT;   break;
            case EQ:        r = MOVE;    break;
            case BUTON0:    r = LEFT;    break;
            case BUTON100:  r = RIGHT;   break;
            case BUTON200:  r = MOVE;    break;
            case BUTON1:    r = LEFT;    break;
            case BUTON2:    r = RIGHT;   break;
            case BUTON3:    r = MOVE;    break;
            case BUTON4:    r = LEFT;    break;
            case BUTON5:    r = RIGHT;   break;
            case BUTON6:    r = MOVE;    break;
            case BUTON7:    r = LEFT;    break;
            case BUTON8:    r = RIGHT;   break;
            case BUTON9:    r = MOVE;    break;
            default:
                            r = MOVE;
            break;
        }
        irRecv.resume();
    } 
    return r;
}
