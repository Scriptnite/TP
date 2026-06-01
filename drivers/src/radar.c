#include "radar.h"
#include "servomotor.h"
#include "ultrasonido.h"
#include "telemetria.h"
#include "config_global.h"
#include "LPC17xx.h"

static RadarMode_T modo_actual;


void RADAR_init(void)
{
    // Inicialización de los perifericos a utilizar
	config_TMR0();
    config_TMR1();
    config_systick();
    // El radar arranca en modo barrido
    modo_actual = RADAR_MODE_SWEEP;
}

void RADAR_setMode(RadarMode_T newMode)
{
	modo_actual = newMode;
}

void RADAR_procces(){
	switch (modo_actual)
	{
	//Modo barrido: el servo se mueve un grado, el trigger recibe una señal de 10us, una vez que el pin echo haya capturado la señal y calculado la
	//distancia, actualiza los datos de telemetria y envia los datos por uart.
	case RADAR_MODE_SWEEP:
		SERVO_step();
		ULTRASONIDO_sendTriggerPulse();
		//Espera a que haya un nuevo dato de distancia disponible
		while (!ULTRASONIDO_isDataReady());
		//Actualiza los datos de telemetria (distancia y angulo)
		TELEMETRIA_actualizar(ULTRASONIDO_getDistance(), SERVO_getAngle());
		//en esta linea iria la funcion que envia los datos por dma. Los datos se encuentra en la instancia del struct Telemetria_T llamado paquete_radar
		break;

	case RADAR_MODE_FIXED_ANGLE:
		//SERVO_setAngle(/* OJO ACAAAAAAA*/);
		ULTRASONIDO_sendTriggerPulse();

		break;
	case RADAR_MODE_TRACKING:
		break;
	}
}
