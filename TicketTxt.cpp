#include "TicketTxt.h"

QString TicketTxt::generar(Venta venta)
{
    QString texto = "PULPERIA LA PULPE\n";
    texto += "Venta #" + QString::number(venta.getNumero()) + "   " + venta.getFecha() + "\n";
    texto += "------------------------------\n";
    for (int i = 0; i < venta.getCantidadLineas(); i++) {
        LineaVenta l = venta.getLinea(i);
        texto += QString::number(l.getCantidad()) + " x " + l.getNombre() + "   "
                 + QString::number(l.getSubtotal()) + "\n";
    }
    texto += "------------------------------\n";
    texto += "Total:  " + QString::number(venta.getTotal()) + "\n";
    texto += "Pago:   " + QString::number(venta.getPago()) + "\n";
    texto += "Cambio: " + QString::number(venta.getCambio()) + "\n";
    return texto;
}

QString TicketTxt::getExtension()
{
    return "txt";
}
