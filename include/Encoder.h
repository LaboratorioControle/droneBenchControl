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

#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "constants.h"

class Encoder {
public:
    Encoder(uint8_t pinA, uint8_t pinB);
    void begin();

    // Ângulo em graus, considerando o offset persistido
    float getAngleDeg() const;

    // Pulsos brutos desde o último reset()
    int32_t getPulseCount() const { return pulseCount; }

    // Zera os pulsos (define zero mecânico desta sessão)
    void reset();

    // Injeta um offset em graus carregado da flash no boot.
    // getAngleDeg() retorna pulsos→graus + offsetDeg.
    void setOffsetDeg(float deg);
    float getOffsetDeg() const { return offsetDeg; }

private:
    uint8_t  pinA;
    uint8_t  pinB;
    volatile int32_t pulseCount;
    float    offsetDeg = 0.0f;

    static void IRAM_ATTR isrA(void* arg);
    static void IRAM_ATTR isrB(void* arg);
};

#endif  // ENCODER_H
