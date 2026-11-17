#pragma once
#include <QString>
#include <vector>
#include "Producto.h"

// Integrante 1: lista de productos y sus reglas.
// Los métodos que pueden fallar devuelven un mensaje de error, o "" si todo salió bien.
class Inventario
{
public:
    QString agregar(Producto producto);
    QString modificar(int posicion, QString nombre, int precio, int existencias);
    void cambiarEstado(int posicion);
    void rebajarExistencias(int posicion, int cantidad);

    int buscar(QString codigo) const;   // devuelve la posición, o -1 si no existe
    int getCantidad() const;
    Producto getProducto(int posicion) const;

private:
    QString validar(QString nombre, int precio, int existencias) const;

    std::vector<Producto> productos;
};
