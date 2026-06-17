package com.scriptnite;

public enum Modos {
	RANGO_DE_ANGULOS("A"),
	ANGULO_ESPECIFICO("B"),
	DISTANCIA_ESPECIFICA("C");

	private final String id;

	Modos(String x) {
		id = x;
	}

	public String getId() {
		return id;
	}

	// Método para convertir desde el valor id ("A", "B", "C")
	public static Modos fromId(String id) {
		for (Modos modo : Modos.values()) {
			if (modo.id.equals(id)) {
				return modo;
			}
		}
		throw new IllegalArgumentException("No existe modo con id: " + id);
	}
}
