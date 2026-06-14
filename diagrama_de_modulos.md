# Sistema de Barrido Ultrasónico - LPC1769

Este diagrama de bloques ilustra la arquitectura de hardware de nuestro proyecto final para Electrónica Digital 3, centrado en el microcontrolador LPC1769 (Cortex-M3). En el diseño se destaca cómo la Matriz de Conectividad GPDMA actúa como el núcleo de automatización, comunicando directamente el Módulo ADC (lectura del potenciómetro de velocidad) con la memoria RAM, y enviando en paralelo los datos procesados hacia el Módulo UART (para el gráfico runtime en la PC) y el Módulo DAC (generador de tonos en el buzzer) sin saturar el procesador. Al mismo tiempo, la CPU se libera para gestionar mediante interrupciones el escaneo del teclado matricial por GPIO, la actualización de la pantalla LCD por el bus I2C y el control de tiempos del servomotor y el sensor ultrasónico HC-SR04 a través del Módulo TIMER.

## Diagrama de Bloques

<img width="850" height="500" alt="Image" src="https://github.com/user-attachments/assets/b06eab5f-0a58-4073-b285-1023772a5aa5" />


## Notas Rápidas de Funcionamiento

* **GPDMA:** Comunica de forma directa el ADC a la RAM, y la RAM hacia la UART (PC) y el DAC (Buzzer) sin intervención continua de la CPU.

* **TIMER:** Genera el PWM para mover el servomotor, el disparo (Trigger) y mide por flanco de captura el retorno `ECHO`
  del HC-SR04.

* **GPDMA (Matriz de Conectividad):** Automatiza la transferencia de datos en background sin intervención de la CPU. Mueve los bloques de memoria asignados a la telemetría directamente hacia los buffers de los periféricos de salida (UART y DAC).

* **TIMER:** Toma la señal PWM a 50 Hz para posicionar el servomotor, genera el pulso de disparo (Trigger) de 10 µs para
  el sensor ultrasónico y utiliza los registros de captura (*Capture*) por hardware para medir el ancho del pulso de
  retorno (ECHO) del HC-SR04 de manera precisa.

* **UART:** Transmite las tramas de datos empaquetadas (Ángulo y Distancia) de forma asíncrona mediante un conversor USB-TTL hacia la PC, permitiendo actualizar la interfaz gráfica (*Dashboard*) a altos frames por segundo (FPS).

* **DAC:** Recibe de forma continua buffers circulares con muestras digitales de audio desde la RAM y las convierte en una señal analógica senoidal para que el buzzer pasivo emita las alertas acústicas de proximidad.

* **GPIO:** Configura las líneas del teclado matricial y realiza el escaneo de filas y columnas. Trabaja asociado a
  interrupciones externas por flanco (EINT) para reaccionar al toque del usuario sin necesidad de un lazo de **polling**
  activo.

* **ADC:** Toma el nivel de señal para calibrar la frecuencia de los pitidos sobre el DAC.

* **I2C:** Envía los comandos y datos de texto hacia el módulo adaptador PCF8574 de la pantalla LCD 16x2. Al usar este protocolo serie de dos hilos (SDA y SCL), se economizan pines físicos del microcontrolador para otras funciones.
