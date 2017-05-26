#include "dcmotor.h"
#include "Arduino.h"

DcMotor::DcMotor(int pin1, int pin2, int pwm)
{
    this->pin1 = pin1;
    this->pin2 = pin2;
    this->pwm = pwm;

    pinMode(pin1, OUTPUT); //输出第一个引脚
    pinMode(pin2, OUTPUT); //输出第二个引脚

    /*
  Pins 5 and 6: controlled by Timer 0 in fast PWM mode (cycle length = 256)

  Setting   Divisor   Frequency
  0x01    1     62500
  0x02      8     7812.5
  0x03      64    976.5625   <--DEFAULT
  0x04    256     244.140625
  0x05    1024    61.03515625

  TCCR0B = (TCCR0B & 0b11111000) | <setting>;
  */
    // TCCR0B = (TCCR0B & 0b11111000) | 5;
    /*
  Pins 9 and 10: controlled by timer 1 in phase-correct PWM mode (cycle length =
  510)

  Setting   Divisor   Frequency
  0x01    1     31372.55
  0x02    8     3921.16
  0x03      64    490.20   <--DEFAULT
  0x04      256     122.55
  0x05    1024    30.64

  TCCR1B = (TCCR1B & 0b11111000) | <setting>;

  Pins 11 and 3:  controlled by timer 2.
    TCCR2B = (TCCR2B & 0b11111000) | <setting>;
  */
    // TCCR1B = (TCCR1B & 0b11111000) | 5;
}

void DcMotor::speed(int val)
{
    val = (val < 0) ? 0 : (val > 255) ? 255 : val;
    analogWrite(pwm, val);
}

void DcMotor::forward()
{
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
}

void DcMotor::backward()
{
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
}

void DcMotor::stop()
{
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
}
