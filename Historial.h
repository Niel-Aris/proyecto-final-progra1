#pragma once
#include <vector>
#include "Venta.h"

// Integrante 3: todas las ventas hechas
class Historial
{
public:
    void agregar(Venta venta);
    int getCantidad() const;
    Venta getVenta(int posicion) const;

private:
    std::vector<Venta> ventas;
};
