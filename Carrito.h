#pragma once
#include <QString>
#include <vector>
#include "LineaVenta.h"
#include "Producto.h"

class Inventario;
class Historial;

// Integrante 2: productos que se van a cobrar en una venta.
// Los métodos que pueden fallar devuelven un mensaje de error, o "" si todo salió bien.
class Carrito
{
public:
    QString agregar(Producto producto, int cantidad);
    QString cobrar(int pago, Inventario* inventario, Historial* historial);
    int getTotal() const;
    int getCantidad() const;
    LineaVenta getLinea(int posicion) const;
    void vaciar();

    // Para editar el carrito desde la pantalla de ventas
    QString cambiarCantidad(int posicion, int cantidad, int existencias);   // 0 quita la línea
    void quitar(int posicion);
    int cantidadDe(QString codigo) const;   // cuántas unidades de ese producto hay en el carrito
    int getUnidades() const;               // total de unidades, sumando todas las líneas

private:
    int buscar(QString codigo) const;

    std::vector<LineaVenta> lineas;
};
