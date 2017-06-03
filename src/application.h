/*
 *
 */

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "driver.h"
#include "ultrasonic.h"
#include <IRremote.h>

class Application {
public:
	static Application& GetApp();
    virtual ~Application();
    bool Init();
    void Run();
    void Log2Serial(const char* str) const;

    enum MOVEMENT { UNDEFINE, LEFT, RIGHT, MOVE };
protected:
    Application();
    Application(const Application&);
    static void onInterrupt();

private:
    void randomWobble(int start = 0, int end = 90);
    void detectObstacle();
    int readIRCode();
    Ultrasonic ultrasonicSensor;
    Driver miniDriver;
    int isObstacleOnLeft;
    int isObstacleOnRight;
    // 红外接收器
    IRrecv irRecv;
};

#endif