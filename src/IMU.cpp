#include "IMU.h"
#include <math.h>

// Accel ±2g → 16384 LSB/g
// Gyro  ±500°/s → 65.5 LSB/(°/s)
static constexpr float ACCEL_SCALE = 16384.0f;
static constexpr float GYRO_SCALE  = 65.5f;

IMU::IMU() : imu(IMU_I2C_ADDR) {}

void IMU::begin() {
    // Pre-check: abort if I2C bus is held low (IMU absent or unpowered)
    pinMode(IMU_SDA, INPUT_PULLUP);
    pinMode(IMU_SCL, INPUT_PULLUP);
    delayMicroseconds(200);
    if (!digitalRead(IMU_SDA) || !digitalRead(IMU_SCL)) {
        Serial.println("[IMU] Barramento I2C em nivel baixo, continuando sem IMU");
        return;
    }

    Wire.begin(IMU_SDA, IMU_SCL);
    Wire.setClock(400000);
    Wire.setTimeOut(10);
    delay(10);

    // Quick probe before initialize()
    Wire.beginTransmission(IMU_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        Serial.println("[IMU] MPU6050 nao encontrado, continuando sem IMU");
        return;
    }

    imu.initialize();
    if (!imu.testConnection()) {
        Serial.println("[IMU] Falha na conexao MPU6050, continuando sem IMU");
        return;
    }

    imu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);   // ±2g
    imu.setFullScaleGyroRange(MPU6050_GYRO_FS_500);    // ±500°/s
    imu.setDLPFMode(MPU6050_DLPF_BW_42);               // 42 Hz LPF

    lastReadUs = micros();
    available  = true;
    Serial.println("[IMU] MPU6050 iniciado");
}

void IMU::attachEncoders(Encoder* pitch, Encoder* yaw) {
    encPitch = pitch;
    encYaw   = yaw;
}

SensorData IMU::read() {
    SensorData data = {};

    if (available) {
        int16_t rawAx, rawAy, rawAz, rawGx, rawGy, rawGz;
        imu.getMotion6(&rawAx, &rawAy, &rawAz, &rawGx, &rawGy, &rawGz);

        data.ax = rawAx / ACCEL_SCALE;
        data.ay = rawAy / ACCEL_SCALE;
        data.az = rawAz / ACCEL_SCALE;
        data.gx = rawGx / GYRO_SCALE;
        data.gy = rawGy / GYRO_SCALE;
        data.gz = rawGz / GYRO_SCALE;

        data.pitch = atan2f(data.ay, data.az) * 180.0f / PI;

        unsigned long now = micros();
        float dt = (now - lastReadUs) / 1000000.0f;
        lastReadUs = now;

        yawAccum += data.gz * dt;
        data.yaw = yawAccum;
    }

    data.encPitchDeg = encPitch ? encPitch->getAngleDeg() : 0.0f;
    data.encYawDeg   = encYaw   ? encYaw->getAngleDeg()   : 0.0f;

    return data;
}
