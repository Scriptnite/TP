#include "../inc/manejo_de_bits.h"
#include "LPC17xx.h"

uint32_t compactarMascara(uint32_t puerto, uint32_t mascara) {
    uint32_t resultado = 0;
    uint8_t indiceResultado = 0;

    for (uint8_t bit = 0; bit < 32; bit++) {
        if (mascara & (1UL << bit)) {
            if (puerto & (1UL << bit)) {
                resultado |= (1UL << indiceResultado);
            }

            indiceResultado++;
        }
    }

    return resultado;
}

uint32_t expandirMascara(uint32_t valorCompactado, uint32_t mascara) {
    uint32_t resultado = 0;
    uint8_t indiceCompactado = 0;

    for (uint8_t bit = 0; bit < 32; bit++) {
        if (mascara & (1UL << bit)) {
            if (valorCompactado & (1UL << indiceCompactado)) {
                resultado |= (1UL << bit);
            }

            indiceCompactado++;
        }
    }

    return resultado;
}

uint8_t contarBitsMascara(uint32_t mascara) {
    uint8_t cantidad = 0;

    while (mascara) {
        cantidad += mascara & 1U;
        mascara >>= 1;
    }

    return cantidad;
}

uint32_t desplazarCircular(uint32_t valor, uint8_t cantidadBits) {
    uint32_t mascara = (1UL << cantidadBits) - 1;

    valor &= mascara;

    uint32_t bitMasSignificativo =
        (valor >> (cantidadBits - 1)) & 1U;

    valor <<= 1;

    valor &= mascara;

    valor |= bitMasSignificativo;

    return valor;
}

uint32_t desplazarMascaraCircular(uint32_t valorPuerto, uint32_t mascara) {
    uint32_t valorCompactado =
        compactarMascara(valorPuerto, mascara);

    uint8_t cantidadBits =
        contarBitsMascara(mascara);

    uint32_t valorDesplazado =
        desplazarCircular(
            valorCompactado,
            cantidadBits
        );

    return expandirMascara(
        valorDesplazado,
        mascara
    );
}

uint8_t buscarPrimerMSB_SET(uint32_t valor) {
    for (int8_t i = 31; i >= 0; i--) {
        if (valor & (1 << i)) return i;
    }

    return 255;
}
