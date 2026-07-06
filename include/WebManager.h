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

#ifndef WEB_MANAGER_H
#define WEB_MANAGER_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <freertos/queue.h>
#include "env.h"
#include "MotorCmd.h"
#include "Calibration.h"

struct CalibCmd {
    enum class Type : uint8_t { NONE, IMU, MOTORS, ENCODERS, RESET } type = Type::NONE;
};

class WebManager {
private:
    AsyncWebServer server;
    AsyncEventSource events;
    const char* ssid;
    const char* password;
    QueueHandle_t motorCmdQueue   = nullptr;
    QueueHandle_t calibCmdQueue   = nullptr;
    QueueHandle_t ctrlParamsQueue = nullptr;   // depth 1, xQueueOverwrite

public:
    WebManager(const char* ssid, const char* pass);
    void begin();
    void attachMotorQueue(QueueHandle_t queue);
    void attachCalibQueue(QueueHandle_t queue);
    void attachCtrlParamsQueue(QueueHandle_t queue);
    void sendTelemetry(const char* json);
    void sendCalibStatus(const char* json);
};

#endif
