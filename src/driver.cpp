
#include "driver.h"
#include "dcmotor.h"
#include <Arduino.h>

Driver::Driver()
    : wheel_left(0)
    , wheel_right(0)
{
}

Driver::~Driver()
{
    if (wheel_left != 0) {
        delete wheel_left;
        wheel_left = 0;
    }
    if (wheel_right != 0) {
        delete wheel_right;
        wheel_right = 0;
    }
}

void Driver::SetupWheel(POSITION pos, int pin1, int pin2, int pwm)
{
    switch (pos) {
    case LEFT:
        // do something
        if (0 == wheel_left)
            wheel_left = new DcMotor(pin1, pin2, pwm);
        break;
    case RIGHT:
        // do something
        if (0 == wheel_right)
            wheel_right = new DcMotor(pin1, pin2, pwm);
        break;
    default:
        // do something
        ;
    }
}

void Driver::forward(int speedVal)
{
    wheel_left->forward();
    wheel_right->forward();

    wheel_left->speed(speedVal);
    wheel_right->speed(speedVal);
}

void Driver::backward(int speedVal)
{
    wheel_left->backward();
    wheel_right->backward();

    wheel_left->speed(speedVal);
    wheel_right->speed(speedVal);
}

void Driver::stop()
{
    wheel_left->stop();
    wheel_right->stop();
}

void Driver::turnLeft(int degree)
{
    stop();
    wheel_left->speed(50);
    wheel_right->speed(50);
    wheel_left->backward();
    wheel_right->forward();
    delay(degree * 8.3);
    stop();
}

void Driver::turnRight(int degree)
{
    stop();
    wheel_left->speed(50);
    wheel_right->speed(50);
    wheel_left->forward();
    wheel_right->backward();
    delay(degree * 8.3);
    stop();
}
