/*
 *
 */

#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "driver.h"
#include "ultrasonic.h"

class Application {
public:
	static Application& GetApp();
    virtual ~Application();
    bool Init();
    void Run();
    void Log(const char* str) const;

protected:
    Application();
    Application(const Application&);
    static void onInterrupt();

private:
    void randomWobble(int start = 0, int end = 90);
    void detectObstacle();
    Ultrasonic ultrasonicSensor;
    Driver miniDriver;
    int isObstacleOnLeft;
    int isObstacleOnRight;
};

#endif