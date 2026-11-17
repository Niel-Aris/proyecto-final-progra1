#include "TicketCsv.h"

QString TicketCsv::generar(Venta venta)
{
    QString texto = "producto,cantidad,precio,subtotal\n";
    for (int i = 0; i < venta.getCantidadLineas(); i++) {
        LineaVenta l = venta.getLinea(i);

        // Si el nombre tiene comas o comillas va entre comillas, como manda el formato CSV
        QString nombre = l.getNombre();
        if (nombre.contains(",") || nombre.contains("\"")) {
            nombre.replace("\"", "\"\"");
            nombre = "\"" + nombre + "\"";
        }

        texto += nombre + "," + QString::number(l.getCantidad()) + ","
                 + QString::number(l.getPrecio()) + "," + QString::number(l.getSubtotal()) + "\n";
    }
    texto += "total,,," + QString::number(venta.getTotal()) + "\n";
    texto += "pago,,," + QString::number(venta.getPago()) + "\n";
    texto += "cambio,,," + QString::number(venta.getCambio()) + "\n";
    return texto;
}

QString TicketCsv::getExtension()
{
    return "csv";
}
