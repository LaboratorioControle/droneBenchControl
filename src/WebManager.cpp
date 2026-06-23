#include "WebManager.h"
#include "constants.h"
#include <ElegantOTA.h>

WebManager::WebManager(const char* apSsid, const char* apPassword)
    : server(80), events("/api/events") {
    this->ssid     = apSsid;
    this->password = apPassword;
}

void WebManager::attachMotorQueue(QueueHandle_t queue) {
    motorCmdQueue = queue;
}

void WebManager::sendTelemetry(const char* json) {
    events.send(json, "telemetry", millis());
}

void WebManager::begin() {
    if (!LittleFS.begin(true)) {
        Serial.println("Erro ao montar o LittleFS!");
        return;
    }
    Serial.println("LittleFS montado com sucesso.");
    Serial.println("Iniciando modo Access Point (AP)...");

    WiFi.softAP(ssid, password);
    IPAddress apIp = WiFi.softAPIP();
    Serial.print("Endereço IP do AP: ");
    Serial.println(apIp);

    // --- Páginas ---
    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/index.html", "text/html");
    });

    // --- LQI params ---
    server.on("/api/params", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(LittleFS, "/parameters.json", "application/json");
    });

    server.on("/api/params", HTTP_POST, [](AsyncWebServerRequest* request) {
        auto get = [&](const char* key) -> String {
            return request->hasParam(key, true)
                ? request->getParam(key, true)->value()
                : "0";
        };

        String json =
            "{\n"
            "    \"k1\": "       + get("k1")       + ",\n"
            "    \"k2\": "       + get("k2")       + ",\n"
            "    \"k3\": "       + get("k3")       + ",\n"
            "    \"k4\": "       + get("k4")       + ",\n"
            "    \"pitch_sp\": " + get("pitch_sp") + ",\n"
            "    \"yaw_sp\": "   + get("yaw_sp")   + "\n"
            "}";

        File f = LittleFS.open("/parameters.json", "w");
        if (!f) {
            request->send(500, "text/plain", "Erro ao abrir arquivo");
            return;
        }
        f.print(json);
        f.close();
        request->send(200, "text/plain", "OK");
    });

    // --- Teste de motor ---
    server.on("/api/test/motor", HTTP_POST, [this](AsyncWebServerRequest* request) {
        if (!motorCmdQueue) {
            request->send(503, "text/plain", "Motor queue not initialized");
            return;
        }

        int duty  = request->hasParam("duty",  true) ? request->getParam("duty",  true)->value().toInt() : 0;
        int motor = request->hasParam("motor", true) ? request->getParam("motor", true)->value().toInt() : 0;

        duty = constrain(duty, -MOTOR_PWM_MAX_DUTY, MOTOR_PWM_MAX_DUTY);

        MotorCmd cmd;
        cmd.mode = MotorCmd::Mode::TEST;
        cmd.dutyPitch = (motor == 0) ? static_cast<float>(duty) : 0.0f;
        cmd.dutyYaw   = (motor == 1) ? static_cast<float>(duty) : 0.0f;

        xQueueOverwrite(motorCmdQueue, &cmd);
        request->send(200, "text/plain", "OK");
    });

    // sets both motors simultaneously — used by the real-time dual sliders
    server.on("/api/test/motors", HTTP_POST, [this](AsyncWebServerRequest* request) {
        if (!motorCmdQueue) {
            request->send(503, "text/plain", "Motor queue not initialized");
            return;
        }

        int dutyPitch = request->hasParam("dutyPitch", true)
            ? request->getParam("dutyPitch", true)->value().toInt() : 0;
        int dutyYaw   = request->hasParam("dutyYaw",   true)
            ? request->getParam("dutyYaw",   true)->value().toInt() : 0;

        dutyPitch = constrain(dutyPitch, -MOTOR_PWM_MAX_DUTY, MOTOR_PWM_MAX_DUTY);
        dutyYaw   = constrain(dutyYaw,   -MOTOR_PWM_MAX_DUTY, MOTOR_PWM_MAX_DUTY);

        MotorCmd cmd;
        cmd.mode      = MotorCmd::Mode::TEST;
        cmd.dutyPitch = static_cast<float>(dutyPitch);
        cmd.dutyYaw   = static_cast<float>(dutyYaw);

        xQueueOverwrite(motorCmdQueue, &cmd);
        request->send(200, "text/plain", "OK");
    });

    server.on("/api/test/stop", HTTP_POST, [this](AsyncWebServerRequest* request) {
        if (motorCmdQueue) {
            MotorCmd cmd;   // mode=TEST, duty=0 nos dois motores
            xQueueOverwrite(motorCmdQueue, &cmd);
        }
        request->send(200, "text/plain", "OK");
    });

    server.on("/api/test/control", HTTP_POST, [this](AsyncWebServerRequest* request) {
        if (motorCmdQueue) {
            MotorCmd cmd;
            cmd.mode = MotorCmd::Mode::CONTROL;
            xQueueOverwrite(motorCmdQueue, &cmd);
        }
        request->send(200, "text/plain", "OK");
    });

    // --- SSE telemetria ---
    events.onConnect([](AsyncEventSourceClient* client) {
        client->send("{}", "telemetry", millis(), 1000);
    });
    server.addHandler(&events);

    // OTA via AP — acessível em 192.168.4.1/update
    ElegantOTA.begin(&server);
    ElegantOTA.onStart([this]() {
        if (motorCmdQueue) {
            MotorCmd stop;
            xQueueOverwrite(motorCmdQueue, &stop);
        }
        Serial.println("[OTA] Iniciando atualizacao — motores parados");
    });
    ElegantOTA.onEnd([](bool success) {
        Serial.printf("[OTA] %s — reiniciando\n", success ? "Sucesso" : "Falhou");
    });

    server.begin();
    Serial.println("Servidor Web iniciado na porta 80.");
}
