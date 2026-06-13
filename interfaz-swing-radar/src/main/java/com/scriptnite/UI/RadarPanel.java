package com.scriptnite.UI;

import javax.swing.*;
import java.awt.*;
import java.util.HashMap;
import java.util.Map;

@SuppressWarnings("FieldCanBeLocal")
public class RadarPanel extends JPanel {
    private static final Color colorLineasGuias = new Color(0, 100, 0);
    private static final int diametroCirculoPixels = 10;
    private final int MAX_DISTANCIA_CM = 220; // Rango máximo del HC-SR04
    private final Map<Integer, Integer> mapaObstaculos = new HashMap<>(); // <Angulo, distancia>
    private int anguloActual = 0;

    public RadarPanel() {
        this.setBackground(Color.BLACK);
    }

    public void actualizarDato(int angulo, int distancia) {
        this.anguloActual = angulo;
        this.mapaObstaculos.put(angulo, distancia);
        this.repaint();
    }

    public void limpiarDatos(){
        mapaObstaculos.clear();
        this.repaint();
    }

    @Override
    protected void paintComponent(Graphics g) {
        super.paintComponent(g);
        Graphics2D g2d = (Graphics2D) g;

        // Activar Antialiasing para que las líneas se vean suaves y no pixeladas
        g2d.setRenderingHint(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON
        );

        int ancho = getWidth();
        int alto = getHeight();
        int centroX = ancho / 2;
        int centroY = alto - 50; // El origen del radar abajo al centro
        int radioMax = Math.min(ancho / 2, alto - 100);

        // Dibuja la rejilla de radar (Líneas guía)
        g2d.setColor(colorLineasGuias);
        // Círculos concéntricos (Escala de distancia)
        for (int r = radioMax / 4; r <= radioMax; r += radioMax / 4) {
            g2d.drawOval(centroX - r, centroY - r, r * 2, r * 2);
        }

        // Líneas de ángulos típicos (45°, 90°, 135°)
        g2d.drawLine(centroX, centroY, centroX - radioMax, centroY);
        g2d.drawLine(centroX, centroY, centroX + radioMax, centroY);
        g2d.drawLine(centroX, centroY, centroX, centroY - radioMax);

        // Dibuja los obstáculos
        g2d.setColor(Color.RED);
        for (Map.Entry<Integer, Integer> entry : mapaObstaculos.entrySet()) {
            int ang = entry.getKey();
            int dist = entry.getValue();

            // Si el sensor no detecta nada o excede el rango, no lo dibujamos de la misma forma
            if (dist > 0 && dist <= MAX_DISTANCIA_CM) {
                // Mapear la distancia real en cm a los píxeles de la pantalla
                double rPixel = ((double) dist / MAX_DISTANCIA_CM) * radioMax;
                double rad = Math.toRadians(ang);

                int xObjeto = (int) (centroX + rPixel * Math.cos(rad));
                int yObjeto = (int) (centroY - rPixel * Math.sin(rad));

                // Dibujar un pequeño círculo rojo como obstáculo
                g2d.fillOval(xObjeto - 4, yObjeto - 4, diametroCirculoPixels, diametroCirculoPixels);
            }
        }

        // Dibujar la línea de barrido
        g2d.setColor(Color.GREEN);
        g2d.setStroke(new BasicStroke(2f));

        double radActual = Math.toRadians(anguloActual);
        int xLinea = (int) (centroX + radioMax * Math.cos(radActual));
        int yLinea = (int) (centroY - radioMax * Math.sin(radActual));

        g2d.drawLine(centroX, centroY, xLinea, yLinea);
    }
}