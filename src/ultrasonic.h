
#ifndef ULTRASONIC_HPP
#define ULTRASONIC_HPP

class Ultrasonic {
public:
    Ultrasonic(int triggerPin, int echoPin, float temperature = 25, unsigned long timeout = 23200);
    int getHitTime();
    float getDistance();
    void setTemperature(float);
    float multipleSampling(const int NumofSamples = 3, const float Precision = .9);

private:
    float K;
    int triggerPin;
    int echoPin;
    unsigned long timeout;

    void triggerPulse();
};

#endif
