package com.scriptnite.Service;

import com.fazecast.jSerialComm.SerialPort;
import org.jetbrains.annotations.NotNull;

import java.util.*;
import java.util.function.Consumer;

public class UartManager {
	private static UartManager instancia;
	private SerialPort puertoActivo;
	private Thread hiloLectura;
	private boolean conectado = false;

	// Callback para avisarle a la UI cuando llega una línea de texto
	private Consumer<String> onDataReceivedCallback;

	public UartManager() {
	}

	public static synchronized UartManager getInstancia() {
		if (instancia == null) instancia = new UartManager();

		return instancia;
	}

	// Configurar el receptor de datos (UI)
	public void setOnDataReceived(Consumer<String> callback) {
		onDataReceivedCallback = callback;
	}

	// Obtener lista de puertos COM disponibles
	public String[] obtenerPuertosDisponibles() {
		SerialPort[] puertos = SerialPort.getCommPorts();
		List<String> nombres = new ArrayList<>();
		for (SerialPort p : puertos) {
			nombres.add(p.getSystemPortName()); // Retorna "COM3", "COM4", etc.
		}
		return nombres.toArray(new String[0]);
	}

	// Conectar al puerto con un Baudrate específico
	public boolean conectar(String nombrePuerto, int baudRate) {
		if (conectado) {
			desconectar();
		}

		puertoActivo = SerialPort.getCommPort(nombrePuerto);
		puertoActivo.setBaudRate(baudRate);
		puertoActivo.setNumDataBits(8);
		puertoActivo.setNumStopBits(1);
		puertoActivo.setParity(0); // Sin paridad

		// Configurar Timeouts: tiempo de lectura corto para no bloquear
		// TIMEOUT_READ_SEMI_BLOCKING: devuelve de inmediato si hay datos, espera si no
		puertoActivo.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 0, 0);

		// Intentar abrir el puerto
		if (puertoActivo.openPort()) {
			puertoActivo.setDTR(); // Activa Data Terminal Ready
			puertoActivo.setRTS(); // Activa Request To Send

			try {
				Thread.sleep(200); // Esperar a que el LPC1769 se reinicie por DTR
			} catch (InterruptedException e) {
				Thread.currentThread().interrupt();
			}

			conectado = true;
			iniciarHiloLectura(); // Arranca el hilo secundario
			return true;
		}
		return false;
	}

	// Desconectar el puerto
	public void desconectar() {
		if (conectado){
			conectado = false;
			if (hiloLectura != null && hiloLectura.isAlive()){
				hiloLectura.interrupt();
			}
			if (puertoActivo != null && puertoActivo.isOpen()){
				puertoActivo.closePort();
			}
		}
	}

	public boolean isConectado() {
		return conectado;
	}

	private void iniciarHiloLectura() {
		hiloLectura = new Thread(() -> {
			StringBuilder lineBuffer = new StringBuilder();
			byte[] buffer = new byte[1024];

			while (conectado && !Thread.currentThread().isInterrupted()) {
				try {
					// Leer bytes disponibles directamente del puerto
					int bytesRead = puertoActivo.readBytes(buffer, buffer.length);

					if (bytesRead > 0) {
						// Convertir bytes a String
						String datos = new String(buffer, 0, bytesRead);
						lineBuffer.append(datos);

						// Procesar líneas completas (separadas por '\n')
						String contenido = lineBuffer.toString();
						int indexNewline;

						while ((indexNewline = contenido.indexOf('\n')) != -1) {
							String linea = contenido.substring(0, indexNewline).trim();

							if (!linea.isEmpty() && onDataReceivedCallback != null) {
								System.out.println("[UART RX] " + linea); // Debug
								onDataReceivedCallback.accept(linea);
							}

							// Eliminar la línea procesada del buffer
							contenido = contenido.substring(indexNewline + 1);
						}

						lineBuffer = new StringBuilder(contenido);
					}

					//noinspection BusyWait
					Thread.sleep(10); // Pequeño respiro para la CPU

				} catch (Exception e) {
					System.err.println("Error en el hilo de lectura UART: " + e.getMessage());
					e.printStackTrace();
					conectado = false;
					break;
				}
			}
		});
		hiloLectura.setName("Hilo-UART-LPC1769");
		hiloLectura.start();
	}

	public @NotNull List<String> getPuertosDisponibles() {
		List<String> puertos = new ArrayList<>();

		// Escanear los puertos del sistema operativo (retorna un array)
		SerialPort[] puertosDetectados = SerialPort.getCommPorts();

		for (SerialPort p : puertosDetectados){
			puertos.add(p.getSystemPortName());
			System.out.println(p.getSystemPortName());
		}

		return  puertos;
	}
}