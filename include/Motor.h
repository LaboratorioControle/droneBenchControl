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

#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <freertos/portmacro.h>
#include "constants.h"

class Motor {
public:
    Motor(uint8_t rpwm, uint8_t lpwm, uint8_t ledcChRpwm, uint8_t ledcChLpwm);
    void begin();

    // Aplica sinal de controle com compensação de deadband.
    // range [-MOTOR_PWM_MAX_DUTY, +MOTOR_PWM_MAX_DUTY]
    void setVelocidade(float sinalControle);

    // Versão sem deadband — usada internamente pela calibração.
    void setVelocidadeRaw(float sinalControle);

    void parar();

    void setDeadband(float fwd, float rev);

private:
    uint8_t pinRPWM;
    uint8_t pinLPWM;
    uint8_t chRPWM;
    uint8_t chLPWM;
    float deadbandFwd = 0.0f;
    float deadbandRev = 0.0f;
    portMUX_TYPE mux  = portMUX_INITIALIZER_UNLOCKED;

    void applyDuty(float sinal);
};

#endif  // MOTOR_H
