#include "Carrito.h"

QString Carrito::agregar(Producto producto, int cantidad)
{
    if (producto.getActivo() == false) {
        return "El producto no está activo.";
    }
    if (cantidad <= 0) {
        return "La cantidad debe ser mayor que cero.";
    }

    int posicion = buscar(producto.getCodigo());
    int enCarrito = 0;
    if (posicion != -1) {
        enCarrito = lineas[posicion].getCantidad();
    }

    if (enCarrito + cantidad > producto.getExistencias()) {
        return "No hay existencias suficientes.";
    }

    if (posicion == -1) {
        lineas.push_back(LineaVenta(producto.getCodigo(), producto.getNombre(), cantidad, producto.getPrecio()));
    } else {
        lineas[posicion].setCantidad(enCarrito + cantidad);
    }
    return "";
}

int Carrito::getTotal() const
{
    int total = 0;
    for (int i = 0; i < lineas.size(); i++) {
        total += lineas[i].getSubtotal();
    }
    return total;
}

int Carrito::getCantidad() const
{
    return lineas.size();
}

LineaVenta Carrito::getLinea(int posicion) const
{
    return lineas[posicion];
}

void Carrito::vaciar()
{
    lineas.clear();
}

int Carrito::buscar(QString codigo) const
{
    for (int i = 0; i < lineas.size(); i++) {
        if (lineas[i].getCodigo() == codigo) {
            return i;
        }
    }
    return -1;
}
