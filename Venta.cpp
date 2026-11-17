#include "Venta.h"

Venta::Venta(int numero, QString fecha, int pago)
{
    this->numero = numero;
    this->fecha = fecha;
    this->pago = pago;
}

void Venta::agregarLinea(LineaVenta linea)
{
    lineas.push_back(linea);
}

int Venta::getNumero() const
{
    return numero;
}

QString Venta::getFecha() const
{
    return fecha;
}

int Venta::getPago() const
{
    return pago;
}

int Venta::getCantidadLineas() const
{
    return lineas.size();
}

LineaVenta Venta::getLinea(int posicion) const
{
    return lineas[posicion];
}

int Venta::getTotal() const
{
    int total = 0;
    for (int i = 0; i < lineas.size(); i++) {
        total = total + lineas[i].getSubtotal();
    }
    return total;
}

int Venta::getCambio() const
{
    return pago - getTotal();
}
