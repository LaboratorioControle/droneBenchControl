/*
 * Bench Drone Control Firmware
 * Copyright (C) 2026  Joao Vitor Santos Barbosa, Paulo Junio Fernandes Braga
 * Developed for course GAT125 - Laboratório Integrador,
 * Class 22A (2026/01)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef IMU_H
#define IMU_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include "constants.h"
#include "Encoder.h"

struct SensorData {
    // acelerômetro (g) e giroscópio (°/s) — já com offset aplicado
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

    // Leitura normal com offset de calibração aplicado
    SensorData read();

    // Leitura bruta sem offset — usada pela rotina de calibração
    SensorData readRaw();

    // Injeta offsets calculados pela Calibration
    void setCalibration(float offAx, float offAy, float offAz,
                        float offGx, float offGy, float offGz);

    // Zera o acumulador de yaw — chamar após calibração ou reset
    void resetYaw();

private:
    Adafruit_MPU6050 imu;

    Encoder* encPitch = nullptr;
    Encoder* encYaw   = nullptr;

    bool available = false;
    float yawAccum = 0.0f;
    unsigned long lastReadUs = 0;

    // Offsets de calibração
    float offAx = 0.0f, offAy = 0.0f, offAz = 0.0f;
    float offGx = 0.0f, offGy = 0.0f, offGz = 0.0f;

    SensorData readInternal(bool applyOffset);
};

#endif
