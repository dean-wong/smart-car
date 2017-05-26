#ifndef DcMotor_h
#define DcMotor_h

class DcMotor {
  private:
    int pin1;
    int pin2;
    int pwm;
    
  public:
    DcMotor(int pin1, int pin2, int pwm);
    void speed(int);
    void stop();
    void forward();
    void backward();
};
#endif
