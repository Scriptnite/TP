package com.scriptnite.UI;

import java.awt.*;
import javax.swing.*;

public class Ventana extends JFrame {
	View view;

	public Ventana(String title, Dimension size) throws HeadlessException {
		super(title);

		view = new View(true);

		this.setSize(size);
		this.setPreferredSize(size);
		this.setMinimumSize(size);
		this.setLayout(new BorderLayout());
		this.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		this.setContentPane(view.getPanelPrincipal());
		this.setVisible(true);

		this.setResizable(false);
	}

	public View getView() {
		return view;
	}
}
