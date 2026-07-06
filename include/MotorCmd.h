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

#ifndef MOTOR_CMD_H
#define MOTOR_CMD_H

#include <Arduino.h>

// Modo de operação da task de controle
enum class ControlMode : uint8_t {
    OPEN_LOOP = 0,  // malha aberta: duty fixo enviado pela web
    DOF1      = 1,  // 1-DOF: controla apenas pitch
    DOF2      = 2   // 2-DOF: controla pitch e yaw simultaneamente
};

// Parâmetros do controlador — enviados pela web via ctrlParamsQueue.
// Todos os ângulos/referências em RADIANOS dentro do controlador.
// A web envia em graus; a conversão é feita no WebManager antes de enfileirar.
struct ControlParams {
    ControlMode mode = ControlMode::DOF1;

    // ── Referências ──────────────────────────────────────────────────────
    float refPitch = 0.0f;   // rp  [rad]
    float refYaw   = 0.0f;   // ry  [rad]

    // ── Ganhos integradores ──────────────────────────────────────────────
    // 1-DOF: Ki1 (escalar)
    // 2-DOF: matriz Ki (2×2) linha a linha
    //
    //   Ki = [Ki1  Ki2]     uPitch ← Ki1·∫(rp−θ) + Ki2·∫(ry−Ω)
    //        [Ki3  Ki4]     uYaw   ← Ki3·∫(rp−θ) + Ki4·∫(ry−Ω)
    float Ki1 = 0.0f;   // linha 1, col 1  ∫pitch → uPitch
    float Ki2 = 0.0f;   // linha 1, col 2  ∫yaw   → uPitch
    float Ki3 = 0.0f;   // linha 2, col 1  ∫pitch → uYaw
    float Ki4 = 0.0f;   // linha 2, col 2  ∫yaw   → uYaw

    // ── Ganhos de estado ─────────────────────────────────────────────────
    // 1-DOF: Kx1 (θ), Kx2 (θ̇)
    // 2-DOF: linha pitch: Kx1(θ) Kx2(Ω) Kx3(θ̇) Kx4(Ω̇)
    //         linha yaw : Kx5(θ) Kx6(Ω) Kx7(θ̇) Kx8(Ω̇)
    float Kx1 = 0.0f, Kx2 = 0.0f, Kx3 = 0.0f, Kx4 = 0.0f;
    float Kx5 = 0.0f, Kx6 = 0.0f, Kx7 = 0.0f, Kx8 = 0.0f;

    // ── Malha aberta ─────────────────────────────────────────────────────
    float openLoopPitch = 0.0f;   // duty fixo pitch [-255, +255]
    float openLoopYaw   = 0.0f;   // duty fixo yaw
};

// Comando de acionamento direto dos motores (modo TEST de calibração/teste)
struct MotorCmd {
    enum class Mode : uint8_t { CONTROL, TEST, CALIBRATION } mode = Mode::CONTROL;
    float dutyPitch = 0.0f;
    float dutyYaw   = 0.0f;
};

#endif
