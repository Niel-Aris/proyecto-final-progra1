#include "Carrito.h"
#include <QDate>
#include "Inventario.h"
#include "Historial.h"
#include "Venta.h"

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

QString Carrito::cobrar(int pago, Inventario* inventario, Historial* historial)
{
    if (lineas.size() == 0) {
        return "El carrito está vacío.";
    }
    if (pago < getTotal()) {
        return "El pago no es suficiente.";
    }

    for (int i = 0; i < lineas.size(); i++) {
        int posicion = inventario->buscar(lineas[i].getCodigo());
        if (posicion == -1) {
            return "El producto " + lineas[i].getNombre() + " ya no existe.";
        }
        if (inventario->getProducto(posicion).getActivo() == false) {
            return "El producto " + lineas[i].getNombre() + " no está activo.";
        }
        if (lineas[i].getCantidad() > inventario->getProducto(posicion).getExistencias()) {
            return "No hay existencias suficientes de " + lineas[i].getNombre() + ".";
        }
    }

    for (int i = 0; i < lineas.size(); i++) {
        int posicion = inventario->buscar(lineas[i].getCodigo());
        inventario->rebajarExistencias(posicion, lineas[i].getCantidad());
    }

    Venta venta(historial->getCantidad() + 1, QDate::currentDate().toString("dd/MM/yyyy"), pago);
    for (int i = 0; i < lineas.size(); i++) {
        venta.agregarLinea(lineas[i]);
    }
    historial->agregar(venta);
    vaciar();
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
