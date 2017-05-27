/*
 *
 */

#include "ultrasonic.h"
#include <Arduino.h>

Ultrasonic::Ultrasonic(int triggerPin, int echoPin, float temperature, unsigned long timeout)
{
    this->triggerPin = triggerPin;
    this->echoPin = echoPin;
    this->timeout = timeout;

    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);

    setTemperature(temperature);
}

int Ultrasonic::getHitTime()
{
    // Send a 10us pulse to the TRIG pin
    triggerPulse();

    // Calculate the elapsed time for the sound to go and return
    int pulseDuration = (int)pulseIn(echoPin, HIGH, timeout);

    // If the pulse duration is 0, timeout happened
    if (pulseDuration == 0) {
        return 0;
    }

    // Divide by two, because we are only counting the time the pulse took to go
    int timeToHit = pulseDuration / 2;

    // Return the result
    return timeToHit;
}

void Ultrasonic::triggerPulse()
{
    // Make sure the trigger pin is low
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);

    // Send a 10us pulse (as described in the HC-SR04 datasheet)
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
}

void Ultrasonic::setTemperature(float temp)
{
    // 按往返计算加入温度的距离系数：
    // K = 20000/(332+(0.607*temp));
    
    // 然后将这个系数代人距离计算公式：
    // distance = pulseIn(EchoPin, HIGH) / K；
    this->K = 10000 / (332 + 0.607 * temp);
}

float Ultrasonic::getDistance()
{
    int hitTime = getHitTime();

    return hitTime / K;
}

/*
 * 单次获取的数据漂移太大，多次测距之后再返回平均值
 * 如果多次测距的方差太大，说明无效，返回小于0的值
 */
float Ultrasonic::multipleSampling(const int NumofSamples, const float Precision)
{
    float* Samples = new float[NumofSamples];
    float sum = 0.0, aver = 0.0, e = 0.0;

    for (int i = 0; i < NumofSamples; i++) {
        Samples[i] = getDistance();
        sum += Samples[i];

        // 检测脉冲宽度，并计算出距离，为了防止反射波干扰，两次检测中间需要延时60ms以上。
        // https://wenku.baidu.com/view/ce9e5e48767f5acfa1c7cd8a.html
        delay(60);
    }

    // 多个返回值已经读取完毕，计算结果是否符合要求
    aver = sum / NumofSamples;
    for (int i = 0; i < NumofSamples; i++) {
        e += (Samples[i] - aver) * (Samples[i] - aver);
    }
    e /= (NumofSamples - 1);
    float s = sqrt(e);

    delete Samples;

    // 标准差小于0.3，就认为比较稳定了，可以将均值作为最终结果
    // 实际应用中，可以调整这个参数值，达到理想的结果
    return (s < Precision) ? aver : -1.0;
}
