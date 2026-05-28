package com.scriptnite.Service;

import com.fazecast.jSerialComm.SerialPort;
import org.jetbrains.annotations.NotNull;

import java.io.BufferedReader;
import java.io.InputStreamReader;
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

		// Configurar Timeouts para evitar que el hilo se quede eternamente trabado
		puertoActivo.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 100, 0);

		// Intentar abrir el puerto
		if (puertoActivo.openPort()) {
			puertoActivo.setDTR(); // Activa Data Terminal Ready
			puertoActivo.setRTS(); // Activa Request To Send

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
			try (BufferedReader reader = new BufferedReader(new InputStreamReader(puertoActivo.getInputStream()))) {
				while (conectado && !Thread.currentThread().isInterrupted()) {
					// Se queda bloqueado aquí esperando un salto de línea '\n' del LPC1769
					if (reader.ready() || puertoActivo.bytesAvailable() > 0){
						String linea = reader.readLine();
						if (linea != null && !linea.trim().isEmpty()){
							// Si hay un callback registrado, le mandamos la línea recibida
							if (onDataReceivedCallback != null){
								onDataReceivedCallback.accept(linea);
							}
						}
					}
					//noinspection BusyWait
					Thread.sleep(10); // Pequeño respiro para la CPU
				}
			} catch (Exception e) {
				System.err.println("Error en el hilo de lectura UART: " + e.getMessage());
				conectado = false;
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