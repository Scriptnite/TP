package com.scriptnite.UI;

import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.*;

import com.scriptnite.Modos;
import com.scriptnite.Service.UartManager;

public class View extends JPanel {
	private static final UartManager instanciaUART = UartManager.getInstancia();
	private JPanel panelPrincipal;
	private JPanel contentPanelLeft;
	private JPanel contentPanelRigth;
	private JSlider slider1;
	private JPanel contentGraphPanel;
	private JTextField anguloInicialTextField;
	private JTextField anguloFinalTextField;
	private JTextField distanciaEspecificaTextField;
	private JRadioButton distanciaEspecificaRadioButton;
	private JRadioButton rangoAngulosRadioButton;
	private JRadioButton anguloEspecificoRadioButton;
	private JLabel anguloEspecificoLabel;
	private JLabel rangoAngulosLabel;
	private JLabel anguloInicialLabel;
	private JLabel anguloFinalLabel;
	private JLabel distanciaEspecificaLabel;
	private JLabel cmLabel;
	private JPanel rangoDeAngulosContainerPanel;
	private JPanel distanciaEspecificaContainerPanel;
	private JPanel modosContainerPanel;
	private JLabel resultadoTitleLabel;
	private JPanel configsConsoleContainerPanel;
	private JTextArea UARTConsoleTextArea;
	private JLabel resultadoContentLabel;
	private JComboBox<String> puertoComboBox;
	private JComboBox<String> baudiosComboBox;
	private JButton limpiarDatosButton;
	private JScrollPane containerUARTConsoleScrollPane;
	private final RadarPanel radarPanel = new RadarPanel();

	// Variables para almacenar datos UART del nuevo formato
	private Modos modo;
	private int distancia;
	private int angulo0;
	private int angulo1;
	private int distancia_actual;
	private int angulo_actual;

	public View(boolean isDoubleBuffered) {
		super(isDoubleBuffered);
		initComponents();
		initListeners();

		contentGraphPanel.add(radarPanel);
	}

	void initComponents() {
		/* Init puerto Combo Box */
		List<String> puertosDisponibles = instanciaUART.getPuertosDisponibles();
		puertoComboBox.addItem(null);

		for (String puerto : puertosDisponibles) {
			puertoComboBox.addItem(puerto);
		}

		/* Init baudios Combo Box */
		baudiosComboBox.setSelectedItem("115200");


		DefaultCaret caret = (DefaultCaret) UARTConsoleTextArea.getCaret();
		caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
	}

	void initListeners() {
		/* Listener puerto Combo Box */
		puertoComboBox.addActionListener(_ -> {
			String puertoSeleccionado = (String) puertoComboBox.getSelectedItem();

			if (puertoSeleccionado == null) return;


			int baudios = Integer.parseInt((String) Objects.requireNonNull(baudiosComboBox.getSelectedItem()));

			// 1. CONFIGURAR EL CALLBACK PRIMERO (Antes de conectar)
			instanciaUART.setOnDataReceived(linea -> {
				javax.swing.SwingUtilities.invokeLater(() -> {
					// Mostrar en la consola de texto de la UI
					UARTConsoleTextArea.append("[RX] " + linea + "\n");

					// Procesar y actualizar el radar
					try {
						String[] partes = linea.split(",");

						// Nuevo formato: <modo>,<distancia>,<angulo0>,<angulo1>,<distancia_actual>,<angulo_actual>
						modo = Modos.fromId(partes[0].trim());
						distancia = Integer.parseInt(partes[1].trim());
						angulo0 = Integer.parseInt(partes[2].trim());
						angulo1 = Integer.parseInt(partes[3].trim());
						distancia_actual = Integer.parseInt(partes[4].trim());
						angulo_actual = Integer.parseInt(partes[5].trim());

						// Actualizar el radar con los datos actuales
						radarPanel.actualizarDato(angulo_actual, distancia_actual);
					} catch (Exception ex) {
						UARTConsoleTextArea.append("[ERROR PARSEO] " + ex.getMessage() + "\n");
					}

					UARTConsoleTextArea.setCaretPosition(UARTConsoleTextArea.getDocument().getLength());


					if (modo == Modos.DISTANCIA_ESPECIFICA){
						resultadoContentLabel.setText("Distancia mas cercana a " + distancia + " cm es " + distancia_actual + "cm en el angulo "  + angulo_actual);
					} else if(modo == Modos.RANGO_DE_ANGULOS){
						resultadoContentLabel.setText("Moviendo el servo entre " + angulo0 + " y " + angulo1 + "\n Distancia medida: " + distancia_actual);
					}else {
						resultadoContentLabel.setText("Midiendo en el angulo " + angulo_actual + "\n Distancia medida: " + distancia_actual);
					}

				});
			});

			// 2. CONECTAR SEGUNDO (Ya con el DTR/RTS configurados adentro del UartManager)
			boolean exito = instanciaUART.conectar(puertoSeleccionado, baudios);

			if (exito){
				JOptionPane.showMessageDialog(this, "Conectado exitosamente a la LPC1769");
			} else {
				JOptionPane.showMessageDialog(this, "Error al abrir el puerto", "Error", JOptionPane.ERROR_MESSAGE);
			}
		});

		/* Listener limpiar Datos Button */
		limpiarDatosButton.addActionListener(_ -> {
			radarPanel.limpiarDatos();
			UARTConsoleTextArea.setText("");
		});
	}

	public JPanel getPanelPrincipal() {
		return panelPrincipal;
	}

	public RadarPanel getRadarPanel() {
		return radarPanel;
	}

	public Modos getModo() {
		return modo;
	}

	public int getDistancia() {
		return distancia;
	}

	public int getAngulo0() {
		return angulo0;
	}

	public int getAngulo1() {
		return angulo1;
	}

	public int getDistancia_actual() {
		return distancia_actual;
	}

	public int getAngulo_actual() {
		return angulo_actual;
	}
}