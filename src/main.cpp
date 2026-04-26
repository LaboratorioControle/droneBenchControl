#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Wire.h> // Biblioteca base caso use a IMU via I2C

// ==========================================
// MAPEAMENTO DE HARDWARE (PINOUT)
// ==========================================
// Pinos dos Motores (Ponte H)
#define PIN_MOTOR_PITCH_PWM  18  // Sinal de velocidade (Arfagem)
#define PIN_MOTOR_PITCH_DIR  19  // Sinal de direção (Cima/Baixo)
#define PIN_MOTOR_YAW_PWM    21  // Sinal de velocidade (Guinada)
#define PIN_MOTOR_YAW_DIR    22  // Sinal de direção (Esquerda/Direita)

// Pinos dos Encoders 
#define PIN_ENC_PITCH_A 34
#define PIN_ENC_PITCH_B 35
#define PIN_ENC_YAW_A   32
#define PIN_ENC_YAW_B   33

// ==========================================
// VARIÁVEIS GLOBAIS DE ESTADO E CONTROLE
// ==========================================
// Setpoints
volatile float setpoint_pitch = 0.0;
volatile float setpoint_yaw = 0.0;

// Leitura atual dos sensores
volatile float angulo_pitch = 0.0;
volatile float angulo_yaw = 0.0;

// Parâmetros de Controle - Os valores serão fornecidos pela equipe de MCM
float Kp_pitch = 0.0, Ki_pitch = 0.0, Kd_pitch = 0.0;
float Kp_yaw = 0.0, Ki_yaw = 0.0, Kd_yaw = 0.0;

// ==========================================
// HANDLES DO FREERTOS
// ==========================================
TaskHandle_t TaskControle;
TaskHandle_t TaskTelemetria;

// ==========================================
// FUNÇÕES DE INICIALIZAÇÃO DE HARDWARE
// ==========================================
void inicializarMotores() {
  // Configuração dos pinos de direção
  pinMode(PIN_MOTOR_PITCH_DIR, OUTPUT);
  pinMode(PIN_MOTOR_YAW_DIR, OUTPUT);
  
  // No ESP32, o PWM é configurado de forma nativa e eficiente
  // As funções específicas vão depender do driver de motor escolhido)
  pinMode(PIN_MOTOR_PITCH_PWM, OUTPUT);
  pinMode(PIN_MOTOR_YAW_PWM, OUTPUT);
  
  Serial.println("[Hardware] Motores inicializados.");
}

void inicializarSensores() {
  pinMode(PIN_ENC_PITCH_A, INPUT_PULLUP);
  pinMode(PIN_ENC_PITCH_B, INPUT_PULLUP);
  // Aqui  entrarão as interrupções (attachInterrupt) ou a inicialização da IMU: Wire.begin();
  
  Serial.println("[Hardware] Sensores (Encoders/IMU) inicializados.");
}

// ==========================================
//  CORE 1: MALHA DE CONTROLE 
// ==========================================
void loopControle(void *pvParameters) {
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = pdMS_TO_TICKS(10); // Frequência de 100Hz (10ms)

  for (;;) {
    // Desenvolver futuramente:

        // 1. LEITURA (Feedback): Atualizar angulo_pitch e angulo_yaw
        
        // 2. CÁLCULO DE ERRO: Erro = Setpoint - Angulo
        
        // 3. ALGORITMO MCM: PID
        
        // 4. ATUAÇÃO: PWM nos motores
    
    // Delay para garantir amostragem  em 100Hz
    vTaskDelayUntil(&xLastWakeTime, xFrequency); 
  }
}

// ==========================================
// CORE 0: TELEMETRIA E MATLAB
// ==========================================
void loopTelemetria(void *pvParameters) {
  for (;;) {
    // Envia os dados para a porta Serial no formato que o MATLAB/Simulink consegue ler
    // Exemplo: "Pitch,Yaw\n"
    Serial.printf("%.2f,%.2f\n", angulo_pitch, angulo_yaw);
    
    // Aguarda 50ms (20Hz) para não saturar a porta serial
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

// ==========================================
// SETUP INICIAL DO SISTEMA
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== Aero 2 - ESE Iniciando ===");

  //Setup de Hardware
  inicializarMotores();
  inicializarSensores();

  //Criação das Threads (FreeRTOS)
  // Task de Controle no CORE 1 
  xTaskCreatePinnedToCore(
      loopControle, "TaskControle", 4096, NULL, 2, &TaskControle, 1);

  // Task de Telemetria no CORE 0
  xTaskCreatePinnedToCore(
      loopTelemetria, "TaskTelemetria", 2048, NULL, 1, &TaskTelemetria, 0);

  Serial.println("=== Sistema Operacional de Tempo Real (RTOS) Ativo ===");
}

// ==========================================
// LOOP PADRÃO (Inativo)
// ==========================================
void loop() {
  // Como usa o FreeRTOS (Tasks), o loop nativo fica vazio.
  vTaskDelete(NULL); 
}