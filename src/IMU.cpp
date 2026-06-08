#include "IMU.h"
#include <math.h>

IMU::IMU() {}

void IMU::begin() {
    // Verifica o estado do barramento antes de inicializar Wire
    // Evita travar se o IMU estiver ausente ou sem alimentacao
    pinMode(IMU_SDA, INPUT_PULLUP);
    pinMode(IMU_SCL, INPUT_PULLUP);
    delayMicroseconds(200);
    if (!digitalRead(IMU_SDA) || !digitalRead(IMU_SCL)) {
        Serial.println("[IMU] Barramento I2C em nivel baixo, continuando sem IMU");
        return;
    }

    Wire.begin(IMU_SDA, IMU_SCL);
    Wire.setClock(100000);
    Wire.setTimeOut(10);
    delay(10);  // MPU-6050 needs ~10ms after VDD before I2C is ready

    // Probe rapido: so chama imu.begin() se o dispositivo responder
    Wire.beginTransmission(IMU_I2C_ADDR);
    if (Wire.endTransmission() != 0) {
        Serial.println("[IMU] MPU-6050 nao encontrado, continuando sem IMU");
        return;
    }

    if (!imu.begin(IMU_I2C_ADDR, &Wire)) {
        Serial.println("[IMU] Falha ao iniciar MPU-6050, continuando sem IMU");
        return;
    }

    // Configura faixas adequadas para o drone de bancada:
    //   Acelerômetro: ±2g  (máxima resolução para ângulos pequenos)
    //   Giroscópio  : ±500 °/s
    //   DLPF        : 10 Hz (filtragem de vibração mecânica)
    imu.setAccelerometerRange(MPU6050_RANGE_2_G);
    imu.setGyroRange(MPU6050_RANGE_500_DEG);
    imu.setFilterBandwidth(MPU6050_BAND_10_HZ);

    lastReadUs = micros();
    available = true;
    Serial.println("[IMU] MPU-6050 iniciado");
}

void IMU::attachEncoders(Encoder* pitch, Encoder* yaw) {
    encPitch = pitch;
    encYaw   = yaw;
}

SensorData IMU::read() {
    SensorData data = {};

    if (!available) {
        data.encPitchDeg = encPitch ? encPitch->getAngleDeg() : 0.0f;
        data.encYawDeg   = encYaw   ? encYaw->getAngleDeg()   : 0.0f;
        return data;
    }

    sensors_event_t accelEvt, gyroEvt, tempEvt;

    // getEvent() retorna true quando há dados novos
    if (imu.getEvent(&accelEvt, &gyroEvt, &tempEvt)) {
        // Acelerômetro em m/s² — normaliza para g
        float ay = accelEvt.acceleration.y / 9.80665f;
        float az = accelEvt.acceleration.z / 9.80665f;

        // Giroscópio em rad/s — converte para °/s
        float gz = gyroEvt.gyro.z * (180.0f / PI);

        // Pitch: ângulo do eixo Y em relação à vertical
        data.pitch = atan2f(ay, az) * 180.0f / PI;

        // Yaw: integração trapezoidal do giroscópio Z
        unsigned long now = micros();
        float dt = (now - lastReadUs) / 1000000.0f;
        lastReadUs = now;

        yawAccum += gz * dt;
        data.yaw = yawAccum;
    }

    data.encPitchDeg = encPitch ? encPitch->getAngleDeg() : 0.0f;
    data.encYawDeg   = encYaw   ? encYaw->getAngleDeg()   : 0.0f;

    return data;
}
