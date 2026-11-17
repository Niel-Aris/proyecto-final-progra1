#include "Inventario.h"

QString Inventario::validar(QString nombre, int precio, int existencias) const
{
    if (nombre.trimmed() == "") {
        return "El nombre es obligatorio.";
    }
    if (nombre.contains(";")) {
        return "El nombre no puede tener el símbolo ;";
    }
    if (precio <= 0) {
        return "El precio debe ser mayor que cero.";
    }
    if (existencias < 0) {
        return "Las existencias no pueden ser negativas.";
    }
    return "";
}

QString Inventario::agregar(Producto producto)
{
    if (producto.getCodigo().trimmed() == "") {
        return "El código es obligatorio.";
    }
    if (producto.getCodigo().contains(";")) {
        return "El código no puede tener el símbolo ;";
    }
    if (buscar(producto.getCodigo()) != -1) {
        return "Ya existe un producto con ese código.";
    }
    QString error = validar(producto.getNombre(), producto.getPrecio(), producto.getExistencias());
    if (error != "") {
        return error;
    }
    productos.push_back(producto);
    return "";
}

QString Inventario::modificar(int posicion, QString nombre, int precio, int existencias)
{
    QString error = validar(nombre, precio, existencias);
    if (error != "") {
        return error;
    }
    productos[posicion].setNombre(nombre);
    productos[posicion].setPrecio(precio);
    productos[posicion].setExistencias(existencias);
    return "";
}

void Inventario::cambiarEstado(int posicion)
{
    if (productos[posicion].getActivo() == true) {
        productos[posicion].setActivo(false);
    } else {
        productos[posicion].setActivo(true);
    }
}

void Inventario::rebajarExistencias(int posicion, int cantidad)
{
    int nuevas = productos[posicion].getExistencias() - cantidad;
    productos[posicion].setExistencias(nuevas);
}

int Inventario::buscar(QString codigo) const
{
    for (int i = 0; i < productos.size(); i++) {
        if (productos[i].getCodigo() == codigo) {
            return i;
        }
    }
    return -1;
}

int Inventario::getCantidad() const
{
    return productos.size();
}

Producto Inventario::getProducto(int posicion) const
{
    return productos[posicion];
}
