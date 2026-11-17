#include "Historial.h"

void Historial::agregar(Venta venta)
{
    ventas.push_back(venta);
}

int Historial::getCantidad() const
{
    return ventas.size();
}

Venta Historial::getVenta(int posicion) const
{
    return ventas[posicion];
}
