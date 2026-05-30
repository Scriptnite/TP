#include "LPC17xx.h"
#include "servomotor.h"

static volatile RadarMode_T radar_mode;
static ServoRadar_t mi_radar = {
    .current_pulse = 1000, //angle = 0
    .step          = 5, // equivalente a un grado
    .min_pulse     = 1000, // 0 grados
    .max_pulse     = 2000, //180 grados
    .direction     = SWEEP_FORWARD //inicia con sentido horario
};

void RADAR_setMode(RadarMode_T newMode)
{
	radar_mode = newMode;
}


/*void RADAR_proccess(){
	switch (radar_mode)
	{
	case RADAR_MODE_SWEEP:
		RADAR_step();
		break;
	case RADAR_MODE_FIXED_ANGLE:
		break;
	case RADAR_MODE_TRACKING:
		break;
	}
}
*/
void RADAR_step()
{
	// aumenta o disminuye un step
	if (mi_radar.direction == SWEEP_FORWARD)
	{
		mi_radar.current_pulse += mi_radar.step;
	}
	else
	{
	    mi_radar.current_pulse -= mi_radar.step;
	}

	// actualiza de ser necesario la direccion la direccion
	if (mi_radar.current_pulse <= mi_radar.min_pulse)
	{
		mi_radar.current_pulse = mi_radar.min_pulse;
		mi_radar.direction = SWEEP_FORWARD;
	}
	else if (mi_radar.current_pulse >= mi_radar.max_pulse) {
		mi_radar.current_pulse = mi_radar.max_pulse;
	    mi_radar.direction = SWEEP_REVERSE;
	}
	//actualiza el valor en el registro
	LPC_TIM0->MR0 = mi_radar.current_pulse;
}

bool RADAR_setAngle(uint32_t angle)
{
	if (angle > 180)
	{
		return false; //retorna falso si el angulo es mayor a 180 grados
	}
	mi_radar.current_pulse = 1000 + ((angle * 1000)/180);
	LPC_TIM0->MR0 = mi_radar.current_pulse; //mapeo matematico que cambia el ciclo de trabajo del pwm segun el angulo
	return true;
}




