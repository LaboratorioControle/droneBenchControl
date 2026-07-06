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

#include "Tests.h"

Tests::Tests() {}
Tests::~Tests() {}

void Tests::motor(Motor& obj) {
    Serial.println("[Test::motor] Rampando sentido horario...");
    for (uint16_t i = 0; i <= 255; i++) {
        obj.setVelocidade(i);
        delay(10);
    }
    delay(500);

    Serial.println("[Test::motor] Rampando sentido anti-horario...");
    for (uint16_t i = 0; i <= 255; i++) {
        obj.setVelocidade(-static_cast<int>(i));
        delay(10);
    }
    delay(500);

    obj.setVelocidade(0);
    Serial.println("[Test::motor] Concluido.");
}

void Tests::encoder(Encoder& obj) {
    obj.reset();
    Serial.println("[Test::encoder] Lendo angulo por 5s (gire o eixo)...");
    for (uint8_t i = 0; i < 50; i++) {
        Serial.printf("  %.2f deg\n", obj.getAngleDeg());
        delay(100);
    }
    Serial.println("[Test::encoder] Concluido.");
}

void Tests::imu(IMU& obj) {
    Serial.println("[Test::imu] Lendo dados por 5s...");
    for (uint8_t i = 0; i < 50; i++) {
        SensorData d = obj.read();
        Serial.printf("  pitch: %6.2f deg  yaw: %6.2f deg  encPitch: %6.2f deg  encYaw: %6.2f deg\n",
            d.pitch, d.yaw, d.encPitchDeg, d.encYawDeg);
        delay(100);
    }
    Serial.println("[Test::imu] Concluido.");
}

template<> void Tests::singleTest<Motor>(Motor& obj)     { motor(obj); }
template<> void Tests::singleTest<Encoder>(Encoder& obj) { encoder(obj); }
template<> void Tests::singleTest<IMU>(IMU& obj)         { imu(obj); }
