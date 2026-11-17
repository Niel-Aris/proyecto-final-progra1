#pragma once
#include "Historial.h"

// Guarda y lee las ventas en ventas.txt
// Cada venta ocupa una línea VENTA y luego una línea LINEA por cada producto:
// VENTA;numero;fecha;pago
// LINEA;codigo;nombre;cantidad;precio
// Los métodos son static: se usan sin crear un objeto, así: ArchivoVentas::guardar(historial)
class ArchivoVentas
{
public:
    static bool guardar(Historial* historial);
    static bool cargar(Historial* historial);
};
