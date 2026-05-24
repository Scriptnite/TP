#ifndef HANDLER_H_
#define HANDLER_H_

// Prototipos de los handlers de interrupción para cada periférico. No se implementan todos, solo los necesarios para el proyecto.

void handler_timer0(void);
void handler_timer1(void);
void handler_dma(void);
void handler_uart0(void);
void handler_dac(void);
void handler_gpio(void);
void handler_i2c(void);

#endif /* HANDLER_H_ */