package com.scriptnite;

import java.awt.*;
import javax.swing.*;

import com.scriptnite.UI.CargarTema;
import com.scriptnite.UI.Ventana;

public class Main {
	static Ventana ventana;

	static void main() {
		CargarTema.cargarTema(CargarTema.FLAT_MAC_DARK_LAF);
		ventana = new Ventana(
				"Radar",
				new Dimension(1200, 750)
		);

/*
		new Thread(() -> {
			int angulo = 0;
			boolean subiendo = true;

			while (true) {
				try {
					// 0 a 180 y de vuelta
					if (subiendo){
						angulo += 2; // Avanza de a 2 grados
						if (angulo >= 180) subiendo = false;
					} else {
						angulo -= 2;
						if (angulo <= 0) subiendo = true;
					}

					int distancia = java.util.concurrent.ThreadLocalRandom.current().nextInt(20, 200);

					final int anguloFinal = angulo;
					final int distanciaFinal = distancia;

					SwingUtilities.invokeLater(
							() -> ventana.getView().getRadarPanel()
									.actualizarDato(anguloFinal, distanciaFinal)
					);

					//noinspection BusyWait
					Thread.sleep(50); // 50 ms para que el barrido sea fluido

				} catch (InterruptedException _) {
					System.err.println("Simulación interrumpida");
					break;
				}
			}
		}).start();
		*/
	}
}