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

#include "Motor.h"

Motor::Motor(uint8_t rpwm, uint8_t lpwm, uint8_t ledcChRpwm, uint8_t ledcChLpwm)
    : pinRPWM(rpwm), pinLPWM(lpwm), chRPWM(ledcChRpwm), chLPWM(ledcChLpwm) {}

void Motor::begin() {
    ledcSetup(chRPWM, MOTOR_PWM_FREQ_HZ, MOTOR_PWM_RESOLUTION);
    ledcSetup(chLPWM, MOTOR_PWM_FREQ_HZ, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(pinRPWM, chRPWM);
    ledcAttachPin(pinLPWM, chLPWM);
    parar();
}

void Motor::setDeadband(float fwd, float rev) {
    portENTER_CRITICAL(&mux);
    deadbandFwd = fwd;
    deadbandRev = rev;
    portEXIT_CRITICAL(&mux);
}

void Motor::applyDuty(float sinal) {
    int duty = static_cast<int>(
        constrain(fabsf(sinal), 0.0f, static_cast<float>(MOTOR_PWM_MAX_DUTY)));
    if (sinal > 0.0f) {
        ledcWrite(chRPWM, duty);
        ledcWrite(chLPWM, 0);
    } else if (sinal < 0.0f) {
        ledcWrite(chRPWM, 0);
        ledcWrite(chLPWM, duty);
    } else {
        parar();
    }
}

void Motor::setVelocidadeRaw(float sinalControle) {
    applyDuty(sinalControle);
}

void Motor::setVelocidade(float sinalControle) {
    if (sinalControle == 0.0f) { parar(); return; }

    portENTER_CRITICAL(&mux);
    float dbFwd = deadbandFwd;
    float dbRev = deadbandRev;
    portEXIT_CRITICAL(&mux);

    float db   = (sinalControle > 0.0f) ? dbFwd : dbRev;
    float sign = (sinalControle > 0.0f) ? 1.0f : -1.0f;
    float mag  = fabsf(sinalControle);

    // Mapeia [0, MAX] -> [deadband, MAX] linearmente
    float compensated = db + (mag / MOTOR_PWM_MAX_DUTY) * (MOTOR_PWM_MAX_DUTY - db);
    applyDuty(sign * compensated);
}

void Motor::parar() {
    ledcWrite(chRPWM, 0);
    ledcWrite(chLPWM, 0);
}
