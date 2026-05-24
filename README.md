# Radar por Ultrasonido

## Descripción General

Se desarolla basado en la placa de LPC1769 de NXP. El sistema se basa en el analisis de distancia desde un punto hasta
un objeto a través de un sensor de ultrasonido (HC-SR04). Junto a un Servomotor (SG90) adicionado al Sensor de
Ultransonido, permite que este rote sobre su eje vertical, tomando distancias de objetos en un rango de angulos según
permita el servomotor, en este caso, 180 grados.

Barriendo distancias por ángulos, se envía la información a un software de escritorio, colocados sobre de un gráfico de
“radar”, se hacen visibles los objetos/obtaculos por delante de este.
Así mismo, según la distancia el sistema emite un pitido estable, o una ráfaga de ellos en proporción al rango de
distancia de 20-200 [cm] (la relación pitido-distancia podrá ser regulada a través de un trimmer de ajuste).

Se interactua con el sistema a través de Encoder Mecánico para seleccion de “modos”, con un Teclado Matricial ingresando
los parametros de estos.

### Modos posibles:

* **Por ángulo específico:** Se ingresa el angulo deseado (entre 0º y 180º) y este tomará la distancia que hay al
  obstaculo, si es que existe, en caso de que no, mostrará como que no obstaculo por delante.

* **Por rango de ángulos:** Se ingresan un rango de angulos, hara un barrido entre estos enviando constantemente la
  finromacion al softwate, al terminar, mostrará el rango de distancias tomadas.

* **Por distancia específica:** En este modo, hará un barrido por toda la zona dos veces (irá de 0º a 180º y de vuelta),
  y luego irá al ángulo donde encontró el valor más cercano al ingresado, si no encontró ninguno valor, volver a 0º.

###### Cada valor ingresado en el teclado se confirma con el botón `*`, y se borra con el botón `#`.

## Hardware y Periféricos a Utilizar

* **Sensor de Ultrasonido (HC-SR04):** Se calcula la distancia [cm], midiendo el tiempo [uS] en alto de la señal que
  entrega el pin ECHO y diviendolo por 59, previamente disparando la medicion con una señal en alto durnte 10 [uS].

* **Timer:** Temporizará la Señal que utilice el servomotor para rotar.

* **Pantalla LCD con I2C:** A la hora de la seleccion de los modos, y resultados de los mismos.

* **GPDMA:** Se utilizará para el intercambio rápido de datos con otros perifericos

* **DAC:** Este genera la señal para el buzzer indicando distancia actual.

* **ADC:** Tomará la señal proveniente del trimmer de ajuste.

* **Periférico UART + Software:** Comunicación bidireccional con la computadora, enviando datos de muestras e
  instrucciones y reciben.

## Estructura del Repositorio

* `/src`: Implementation de funciones (.c) declaradas en los headers.
* `/inc`: Archivos “headers” (.h) con las definiciones y prototipos.
* `/utils`: Librerías de abstracción a la placa, para procesamiento de datos.
* `/docs`: Esquemas electrónicos del circuito, y demás.
* `/startup`: Archivos (.c) de in civilization del proyecto, aquí se encuentra el main.