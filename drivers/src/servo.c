#include "../inc/servo.h"

uint16_t SERVO_getAnguloDesdePWM(uint16_t anguloMinimo, uint16_t anguloMaximo, uint32_t anchoPulsoMinimo,
                                 uint32_t anchoPulsoMaximo, uint32_t anchoPulsoActual) {
    // Evitar división por cero si los pulsos límite son iguales
    if (anchoPulsoMaximo <= anchoPulsoMinimo) return anguloMinimo;

    // Control de límites (Clamping) para el pulso actual
    if (anchoPulsoActual <= anchoPulsoMinimo) return anguloMinimo;
    if (anchoPulsoActual >= anchoPulsoMaximo) return anguloMaximo;

    // Cálculo matemático: interpolación lineal con enteros.
    // Multiplicamos primero para mantener la máxima resolución antes de la división.
    const uint32_t deltaPulso = anchoPulsoActual - anchoPulsoMinimo;
    const uint32_t rangoAngulo = anguloMaximo - anguloMinimo;
    const uint32_t rangoPulso = anchoPulsoMaximo - anchoPulsoMinimo;

    return anguloMinimo + (uint16_t)((deltaPulso * rangoAngulo) / rangoPulso);
}


uint32_t SERVO_getPulsoDesdeAngulo(uint16_t anguloMinimo, uint16_t anguloMaximo, uint32_t anchoPulsoMinimo,
                                   uint32_t anchoPulsoMaximo, uint16_t anguloActual) {
    // Evitar división por cero si los ángulos límite son iguales
    if (anguloMaximo <= anguloMinimo) return anchoPulsoMinimo;

    // Control de límites (Clamping) para el ángulo actual
    if (anguloActual <= anguloMinimo) return anchoPulsoMinimo;
    if (anguloActual >= anguloMaximo) return anchoPulsoMaximo;

    // Cálculo matemático: interpolación lineal con enteros.
    // Forzamos el uso de uint32_t/uint64_t en el producto para que no desborde la memoria.
    const uint32_t deltaAngulo = anguloActual - anguloMinimo;
    const uint32_t rangoPulso = anchoPulsoMaximo - anchoPulsoMinimo;
    const uint32_t rangoAngulo = anguloMaximo - anguloMinimo;

    return anchoPulsoMinimo + ((deltaAngulo * rangoPulso) / rangoAngulo);
}