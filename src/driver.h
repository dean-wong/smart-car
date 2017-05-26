#ifndef DRIVER_H
#define DRIVER_H

class DcMotor;

class Driver {
private:
    DcMotor* wheel_left;
    DcMotor* wheel_right;

public:
    Driver();
    virtual ~Driver();
    enum POSITION { LEFT,
        RIGHT };
    void SetupWheel(POSITION pos, int pin1, int pin2, int pwm);
    void forward(int speedVal = 60);
    void backward(int speedVal = 60);
    void stop();
    void turnLeft(int degree);
    void turnRight(int degree);
};

#endif // DRIVER_H
