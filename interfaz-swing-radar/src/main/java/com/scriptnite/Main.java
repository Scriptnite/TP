package com.scriptnite;

import java.awt.*;
import javax.swing.*;

import com.scriptnite.UI.CargarTema;
import com.scriptnite.UI.Ventana;



public class Main {
	static Ventana ventana;
	static boolean simularDatos = false;

	static void main() {
		CargarTema.cargarTema(CargarTema.FLAT_MAC_DARK_LAF);
		ventana = new Ventana(
				"Radar",
				new Dimension(1200, 750)
		);

		if(simularDatos) new Thread(() -> {
			int anguloActual = 0;
			boolean subiendo = true;
			String[] modos = {"A", "B", "C"};

			while (true) {
				try {
					// Barrido de ángulo actual: 0 a 250 y de vuelta
					if (subiendo){
						anguloActual += 2; // Avanza de a 2 grados
						if (anguloActual >= 180) subiendo = false;
					} else {
						anguloActual -= 2;
						if (anguloActual <= 0) subiendo = true;
					}

					// Generar datos simulados en el nuevo formato
					String modo = modos[java.util.concurrent.ThreadLocalRandom.current().nextInt(0, 3)];
					int distancia = java.util.concurrent.ThreadLocalRandom.current().nextInt(0, 220);
					int angulo0 = java.util.concurrent.ThreadLocalRandom.current().nextInt(0, 181);
					int angulo1 = java.util.concurrent.ThreadLocalRandom.current().nextInt(0, 181);
					int distanciaActual = java.util.concurrent.ThreadLocalRandom.current().nextInt(0, 181);

					// Formato: <modo>,<distancia>,<angulo0>,<angulo1>,<distancia_actual>,<angulo_actual>
					String datosFormato = String.format("%s,%d,%d,%d,%d,%d",
							modo, distancia, angulo0, angulo1, distanciaActual, anguloActual);

					final int anguloActualFinal = anguloActual;
					final int distanciaActualFinal = distanciaActual;

					SwingUtilities.invokeLater(
							() -> ventana.getView().getRadarPanel()
									.actualizarDato(anguloActualFinal, distanciaActualFinal)
					);

					// Log de datos enviados (simulación)
					System.out.println("[TX SIM] " + datosFormato);

					//noinspection BusyWait
					Thread.sleep(50); // 50 ms para que el barrido sea fluido

				} catch (InterruptedException _) {
					System.err.println("Simulación interrumpida");
					break;
				}
			}
		}).start();

	}
}