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

#include "Encoder.h"

Encoder::Encoder(uint8_t pinA, uint8_t pinB)
    : pinA(pinA), pinB(pinB), pulseCount(0), offsetDeg(0.0f) {}

void Encoder::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(pinA), isrA, this, CHANGE);
    attachInterruptArg(digitalPinToInterrupt(pinB), isrB, this, CHANGE);
}

// Pino A mudou: direção CW quando a != b
void IRAM_ATTR Encoder::isrA(void* arg) {
    Encoder* enc = static_cast<Encoder*>(arg);
    bool a = digitalRead(enc->pinA);
    bool b = digitalRead(enc->pinB);
    enc->pulseCount += (a != b) ? 1 : -1;
}

// Pino B mudou: direção CW quando a == b
void IRAM_ATTR Encoder::isrB(void* arg) {
    Encoder* enc = static_cast<Encoder*>(arg);
    bool a = digitalRead(enc->pinA);
    bool b = digitalRead(enc->pinB);
    enc->pulseCount += (a == b) ? 1 : -1;
}

float Encoder::getAngleDeg() const {
    float pulsesDeg = (pulseCount / static_cast<float>(ENCODER_PPR_X4)) * 360.0f;
    return pulsesDeg + offsetDeg;
}

void Encoder::reset() {
    pulseCount = 0;
    offsetDeg  = 0.0f;
}

void Encoder::setOffsetDeg(float deg) {
    offsetDeg = deg;
}
