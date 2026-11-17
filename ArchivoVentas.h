#pragma once
#include "Historial.h"

// Guarda y lee las ventas en ventas.json. Cada venta guarda sus líneas con el precio que se usó:
// [ {"numero": 1, "fecha": "17/11/2026 14:46", "pago": 5000,
//    "lineas": [ {"codigo": "P001", "nombre": "Arroz 1 kg", "cantidad": 2, "precio": 1250}, ... ]}, ... ]
// Si todavía no hay ventas.json pero sí el ventas.txt de antes, se lee ese una vez
// y desde la siguiente venta ya queda en JSON.
// Los métodos son static: se usan sin crear un objeto, así: ArchivoVentas::guardar(historial)
class ArchivoVentas
{
public:
    static bool guardar(Historial* historial);
    static bool cargar(Historial* historial);

private:
    static bool cargarTxt(Historial* historial);
};
