#include "Sensor.h"
#include "Constants.h"

Sensor::Sensor() {
    // Construtor, se necessário inicializar algo
}

void Sensor::begin() {
    // Inicialização de hardware do sensor, se houver
    // Ex: Wire.begin(); para I2C
}

DadosSensores Sensor::lerDados() {
    DadosSensores dados;
    // Lógica de leitura real do sensor aqui
    // Por enquanto, um exemplo fake:
    dados.anguloPitch = 15.5; 
    dados.anguloYaw = 0.0;
    return dados;
}
