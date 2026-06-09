#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "constants.h"
#include "Encoder.h"

struct SensorData {
    // raw IMU — acelerômetro (g) e giroscópio (°/s)
    float ax, ay, az;
    float gx, gy, gz;
    // ângulos processados
    float pitch;        // graus — atan2(ay, az)
    float yaw;          // graus — integração gz
    // encoders
    float encPitchDeg;
    float encYawDeg;
};

class IMU {
public:
    IMU();

    void begin();
    void attachEncoders(Encoder* pitch, Encoder* yaw);
    SensorData read();

private:
    Adafruit_MPU6050 imu;

    Encoder* encPitch = nullptr;
    Encoder* encYaw   = nullptr;

    bool available = false;
    float yawAccum = 0.0f;
    unsigned long lastReadUs = 0;
};

#endif
