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

// Cambia la cantidad de una línea, revisando que alcancen las existencias.
// Si la cantidad queda en 0, la línea se quita.
QString Carrito::cambiarCantidad(int posicion, int cantidad, int existencias)
{
    if (cantidad <= 0) {
        quitar(posicion);
        return "";
    }
    if (cantidad > existencias) {
        return "Solo hay " + QString::number(existencias) + " de " + lineas[posicion].getNombre() + ".";
    }
    lineas[posicion].setCantidad(cantidad);
    return "";
}

void Carrito::quitar(int posicion)
{
    lineas.erase(lineas.begin() + posicion);
}

int Carrito::cantidadDe(QString codigo) const
{
    int posicion = buscar(codigo);
    if (posicion == -1) {
        return 0;
    }
    return lineas[posicion].getCantidad();
}

int Carrito::getUnidades() const
{
    int unidades = 0;
    for (int i = 0; i < lineas.size(); i++) {
        unidades = unidades + lineas[i].getCantidad();
    }
    return unidades;
}
