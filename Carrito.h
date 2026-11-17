#pragma once
#include <QString>
#include <vector>
#include "LineaVenta.h"
#include "Producto.h"

// Integrante 2: productos que se van a cobrar en una venta.
// Los métodos que pueden fallar devuelven un mensaje de error, o "" si todo salió bien.
class Carrito
{
public:
    QString agregar(Producto producto, int cantidad);
    int getTotal() const;
    int getCantidad() const;
    LineaVenta getLinea(int posicion) const;
    void vaciar();

private:
    int buscar(QString codigo) const;

    std::vector<LineaVenta> lineas;
};
