package com.scriptnite.UI;

import java.util.logging.*;
import com.formdev.flatlaf.*;
import com.formdev.flatlaf.themes.FlatMacLightLaf;
import com.formdev.flatlaf.themes.FlatMacDarkLaf;
import org.jetbrains.annotations.NotNull;

public enum CargarTema {
	FLAT_LIGHT_LAF,
	FLAT_DARK_LAF,
	FLAT_INTELLI_J_LAF,
	FLAT_DARCULA_LAF,
	FLAT_MAC_LIGHT_LAF,
	FLAT_MAC_DARK_LAF;

	public static void cargarTema(@NotNull CargarTema tema) {
		Logger logger = Logger.getLogger(CargarTema.class.getName());

		switch (tema.ordinal()) {
			case 0:
				if (!FlatLightLaf.setup()){
					logger.warning("No se pudo cargar el tema FlatLightLaf.");
					throw new IllegalStateException("No se pudo cargar el tema FlatLightLaf.");
				}
				break;
			case 1:
				if (!FlatDarkLaf.setup()){
					logger.warning("No se pudo cargar el tema FlatDarkLaf.");
					throw new IllegalStateException("No se pudo cargar el tema FlatDarkLaf.");
				}
				break;
			case 2:
				if (!FlatIntelliJLaf.setup()){
					logger.warning("No se pudo cargar el tema FlatIntelliJLaf.");
					throw new IllegalStateException("No se pudo cargar el tema FlatIntelliJLaf.");
				}
				break;
			case 3:
				if (!FlatDarculaLaf.setup()){
					logger.warning("No se pudo cargar el tema FlatDarculaLaf.");
					throw new IllegalStateException("No se pudo cargar el tema FlatDarculaLaf.");
				}
				break;
			case 4:
				if (!FlatMacLightLaf.setup()){
					logger.warning("No se pudo cargar el tema FlatMacLightLaf.");
					throw new IllegalStateException("No se pudo cargar el tema FlatMacLightLaf.");
				}
				break;
			case 5:
				if (!FlatMacDarkLaf.setup()){
					logger.warning("No se pudo cargar el tema FlatMacDarkLaf.");
					throw new IllegalStateException("No se pudo cargar el tema FlatMacDarkLaf.");
				}
				break;
			default:
				logger.warning("Tema no soportado: " + tema);
				throw new IllegalArgumentException("Tema no soportado: " + tema);
		}
	}
}
