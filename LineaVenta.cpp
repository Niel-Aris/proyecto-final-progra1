#include "LineaVenta.h"

LineaVenta::LineaVenta(QString codigo, QString nombre, int cantidad, int precio)
{
    this->codigo = codigo;
    this->nombre = nombre;
    this->cantidad = cantidad;
    this->precio = precio;
}

QString LineaVenta::getCodigo() const
{
    return codigo;
}

QString LineaVenta::getNombre() const
{
    return nombre;
}

int LineaVenta::getCantidad() const
{
    return cantidad;
}

int LineaVenta::getPrecio() const
{
    return precio;
}

int LineaVenta::getSubtotal() const
{
    return cantidad * precio;
}

void LineaVenta::setCantidad(int cantidad)
{
    this->cantidad = cantidad;
}
