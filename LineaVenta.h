#pragma once
#include <QString>

// Un producto dentro de una venta, con el precio que tenía en ese momento
class LineaVenta
{
public:
    LineaVenta(QString codigo, QString nombre, int cantidad, int precio);

    QString getCodigo() const;
    QString getNombre() const;
    int getCantidad() const;
    int getPrecio() const;
    int getSubtotal() const;

    void setCantidad(int cantidad);

private:
    QString codigo;
    QString nombre;
    int cantidad;
    int precio;
};
