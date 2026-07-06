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

#ifndef TESTS_H
#define TESTS_H

#include <Arduino.h>
#include "Motor.h"
#include "Encoder.h"
#include "IMU.h"

class Tests {
public:
    Tests();
    ~Tests();

    template<typename T>
    void singleTest(T& obj);

private:
    void motor(Motor& obj);
    void encoder(Encoder& obj);
    void imu(IMU& obj);
};

template<> void Tests::singleTest<Motor>(Motor& obj);
template<> void Tests::singleTest<Encoder>(Encoder& obj);
template<> void Tests::singleTest<IMU>(IMU& obj);

#endif
