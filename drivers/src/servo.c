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


uint32_t SERVO_CalcularPaso(uint32_t pclk_hz,
                            uint32_t pr_val,
                            uint32_t pulse_min_us,
                            uint32_t pulse_max_us,
                            uint16_t rango_grados,
                            uint16_t grados_por_paso) {
    // 1. Calculamos el delta de tiempo total del pulso en microsegundos
    const uint32_t delta_pulso_us = pulse_max_us - pulse_min_us;

    // 2. Calculamos el divisor real del clock (PR + 1)
    const uint32_t divisor_timer = pr_val + 1;

    // 3. Aplicamos la fórmula balanceando multiplicaciones primero para no perder resolución.
    // Numerador: Grados_paso * Delta_us * PCLK
    // Denominador: Rango_grados * Divisor * 1.000.000
    const uint64_t numerador = (uint64_t)grados_por_paso * delta_pulso_us * pclk_hz;
    const uint64_t denominador = (uint64_t)rango_grados * divisor_timer * 1000000;

    // Retornamos el paso en Ticks de Timer
    return (uint32_t)(numerador / denominador);
}
