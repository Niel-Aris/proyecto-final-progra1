#include "Producto.h"

Producto::Producto(QString codigo, QString nombre, int precio, int existencias)
{
    this->codigo = codigo;
    this->nombre = nombre;
    this->precio = precio;
    this->existencias = existencias;
    this->activo = true;
}

QString Producto::getCodigo() const
{
    return codigo;
}

QString Producto::getNombre() const
{
    return nombre;
}

int Producto::getPrecio() const
{
    return precio;
}

int Producto::getExistencias() const
{
    return existencias;
}

bool Producto::getActivo() const
{
    return activo;
}

void Producto::setNombre(QString nombre)
{
    this->nombre = nombre;
}

void Producto::setPrecio(int precio)
{
    this->precio = precio;
}

void Producto::setExistencias(int existencias)
{
    this->existencias = existencias;
}

void Producto::setActivo(bool activo)
{
    this->activo = activo;
}
