#ifndef MANEJO_DE_BITS_H
#define MANEJO_DE_BITS_H

#include "LPC17xx.h"


/**
 * @brief Compacta los bits seleccionados por una máscara.
 *
 * Recorre los bits de la máscara desde el menos significativo al más
 * significativo. Cada bit activo de la máscara es copiado al siguiente
 * bit libre del resultado.
 *
 * Ejemplo:
 *
 * puerto  = (1 << 29) | (1 << 15)
 * mascará = (1 << 5) | (1 << 9) | (1 << 15) | (1 << 29)
 *
 * Resultado:
 *
 * 0b1100
 *
 * Correspondencia:
 *
 * resultado[0] <- puerto[5]
 * resultado[1] <- puerto[9]
 * resultado[2] <- puerto[15]
 * resultado[3] <- puerto[29]
 *
 * @param puerto Valor de 32 bits del puerto.
 * @param mascara Máscara de bits a extraer.
 *
 * @return Valor compactado comenzando en el bit 0.
 */
uint32_t compactarMascara(uint32_t puerto, uint32_t mascara);

/**
 * @brief Expande un valor compacto utilizando una máscara.
 *
 * Recorre los bits activos de la máscara desde el menos significativo
 * al más significativo y copia sobre ellos los bits consecutivos del
 * valor compacto.
 *
 * Ejemplo:
 *
 * valorCompactado = 0b1010
 * mascará         = (1 << 5) | (1 << 9) | (1 << 15) | (1 << 29)
 *
 * Resultado:
 *
 * (1 << 9) | (1 << 29)
 *
 * Correspondencia:
 *
 * valorCompactado[0] -> resultado[5]
 * valorCompactado[1] -> resultado[9]
 * valorCompactado[2] -> resultado[15]
 * valorCompactado[3] -> resultado[29]
 *
 * @param valorCompactado Valor cuyos bits comienzan en la posición 0.
 * @param mascara Máscara que indica dónde ubicar cada bit.
 *
 * @return Valor de 32 bits expandido.
 */
uint32_t expandirMascara(uint32_t valorCompactado, uint32_t mascara);

/**
 * @brief Cuenta la cantidad de bits activos de una máscara.
 *
 * @param mascara Máscara a analizar.
 *
 * @return Cantidad de bits en 1.
 */
uint8_t contarBitsMascara(uint32_t mascara);

/**
 * @brief Realiza un desplazamiento circular hacia la izquierda.
 *
 * El desplazamiento se realiza únicamente sobre los bits válidos
 * indicados por cantidadBits.
 *
 * Ejemplo:
 * cantidadBits = 4
 *
 * 0001 -> 0010
 * 0010 -> 0100
 * 0100 -> 1000
 * 1000 -> 0001
 *
 * @param valor Valor compacto.
 * @param cantidadBits Cantidad de bits válidos.
 *
 * @return Valor desplazado circularmente.
 */
uint32_t desplazarCircular(uint32_t valor, uint8_t cantidadBits);

/**
 * @brief Desplaza circularmente los bits contenidos en una máscara.
 *
 * El valor actual se obtiene desde el puerto, se compacta,
 * se desplaza circularmente y luego se reconstruye utilizando
 * la misma máscara.
 *
 * Ejemplo:
 * Máscara:
 * P6 P7 P8 P9
 *
 * Puerto
 * 0100
 *
 * Resultado:
 * 1000
 *
 * @param valorPuerto Valor actual del puerto.
 * @param mascara Máscara de bits a desplazar.
 *
 * @return Valor expandido listo para escribir en el puerto.
 */
uint32_t desplazarMascaraCircular(uint32_t valorPuerto, uint32_t mascara);

/**
 * @brief Busca el bit más significativo en estado alto.
 *
 * @param valor Valor a analizar.
 * @return Posición del bit más significativo que vale 1.
 *         Si valor es 0, retorna 255.
 */
uint8_t buscarPrimerMSB_SET(uint32_t valor);

#endif
