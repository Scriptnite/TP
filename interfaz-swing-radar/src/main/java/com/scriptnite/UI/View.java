package com.scriptnite.UI;

import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.text.*;

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
		baudiosComboBox.setSelectedItem("9600");


		DefaultCaret caret = (DefaultCaret) UARTConsoleTextArea.getCaret();
		caret.setUpdatePolicy(DefaultCaret.ALWAYS_UPDATE);
	}

	void initListeners() {
		/* Listener puerto Combo Box */
		puertoComboBox.addActionListener(_ -> {
			String puertoSeleccionado = (String) puertoComboBox.getSelectedItem();

			// CONTROL ANTINULOS: Si el usuario seleccionó la opción vacía (null), no hacemos nada
			if (puertoSeleccionado == null){
				return;
			}

			int baudios = Integer.parseInt((String) Objects.requireNonNull(baudiosComboBox.getSelectedItem()));

			// 1. CONFIGURAR EL CALLBACK PRIMERO (Antes de conectar)
			instanciaUART.setOnDataReceived(linea -> {
				javax.swing.SwingUtilities.invokeLater(() -> {
					// Mostrar en la consola de texto de la UI
					UARTConsoleTextArea.append("[RX] " + linea + "\n");

					// Procesar y actualizar el radar
					try {
						String[] partes = linea.split(",");
						int angulo = Integer.parseInt(partes[0].trim());
						int distancia = Integer.parseInt(partes[1].trim());

						radarPanel.actualizarDato(angulo, distancia);
					} catch (Exception ex) {
						UARTConsoleTextArea.append("[ERROR PARSEO] " + ex.getMessage() + "\n");
					}

					UARTConsoleTextArea.setCaretPosition(UARTConsoleTextArea.getDocument().getLength());
				});
			});

			// 2. CONECTAR SEGUNDO (Ya con el DTR/RTS configurados adentro del UartManager)
			boolean exito = instanciaUART.conectar(puertoSeleccionado, baudios);

			if (exito){
				JOptionPane.showMessageDialog(this, "Conectado exitosamente al LPC1769");
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
}