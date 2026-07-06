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

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include "Motor.h"
#include "Encoder.h"
#include "IMU.h"

struct CalibData {
    // ── IMU — offsets de bias ──────────────────────────────────────────
    float imuOffsetAx = 0.0f;
    float imuOffsetAy = 0.0f;
    float imuOffsetAz = 0.0f;
    float imuOffsetGx = 0.0f;
    float imuOffsetGy = 0.0f;
    float imuOffsetGz = 0.0f;

    // ── Motor — duty mínimo para vencer atrito estático (dead zone) ───
    float motorPitchDeadbandFwd = 0.0f;
    float motorPitchDeadbandRev = 0.0f;
    float motorYawDeadbandFwd   = 0.0f;
    float motorYawDeadbandRev   = 0.0f;

    // ── Encoders — posição salva no shutdown ──────────────────────────
    // Encoders são incrementais: o zero físico se perde a cada
    // desligamento. Ao desligar de forma controlada, o firmware salva
    // o ângulo atual. No próximo boot, ele é injetado como offset para
    // que o controlador "lembre" onde estava.
    //
    // Se o drone for movido com a energia desligada, o valor salvo
    // estará errado — use "Zerar Encoders" na web para corrigir.
    float encPitchDeg = 0.0f;   // persiste entre boots
    float encYawDeg   = 0.0f;   // persiste entre boots

    // ── Flags ────────────────────────────────────────────────────────
    bool imuCalibrated    = false;
    bool motorCalibrated  = false;
    bool encodersZeroed   = false;  // runtime only — não salvo em JSON
};

class Calibration {
public:
    // Carrega /calibration.json do LittleFS.
    static CalibData load();

    // Persiste todos os campos (exceto encodersZeroed) em /calibration.json.
    static void save(const CalibData& d);

    // Salva apenas as posições dos encoders — chamado pelo shutdown handler.
    static void saveEncoderPositions(const CalibData& d);

    // ── Rotinas de calibração ────────────────────────────────────────

    // IMU: coleta `samples` leituras em repouso e calcula bias.
    static void calibrateIMU(IMU& imu, CalibData& out,
                             int samples = 500, int periodMs = 2);

    // Motores: rampa crescente de duty até encoder detectar movimento.
    static void calibrateMotors(Motor& pitch, Motor& yaw,
                                Encoder& encPitch, Encoder& encYaw,
                                CalibData& out,
                                int stepDuty = 3, int stepMs = 150,
                                int timeoutMs = 8000);

    // Encoders: zera pulseCount e offsetDeg. Posição atual vira 0°.
    // Atualiza calib.encPitchDeg / encYawDeg para 0 e salva.
    static void zeroEncoders(Encoder& encPitch, Encoder& encYaw,
                             CalibData& out);
};

#endif
