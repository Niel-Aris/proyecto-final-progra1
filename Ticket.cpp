#include "Ticket.h"
#include <QFile>
#include <QTextStream>

bool Ticket::guardar(Venta venta)
{
    QString nombre = "ticket-" + QString::number(venta.getNumero()) + "." + getExtension();
    QFile archivo(nombre);
    if (archivo.open(QIODevice::WriteOnly) == false) {
        return false;
    }
    QTextStream salida(&archivo);
    salida << generar(venta);
    archivo.close();
    return true;
}
