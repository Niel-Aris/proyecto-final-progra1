#pragma once
#include <QString>
#include <vector>
#include "LineaVenta.h"

class Venta
{
public:
    Venta(int numero, QString fecha, int pago);

    void agregarLinea(LineaVenta linea);

    int getNumero() const;
    QString getFecha() const;
    int getPago() const;
    int getCantidadLineas() const;
    LineaVenta getLinea(int posicion) const;

    int getTotal() const;
    int getCambio() const;

private:
    int numero;
    QString fecha;
    int pago;
    std::vector<LineaVenta> lineas;
};
