#include "ArchivoVentas.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

bool ArchivoVentas::guardar(Historial* historial)
{
    QFile archivo("ventas.txt");
    if (archivo.open(QIODevice::WriteOnly) == false) {
        return false;
    }
    QTextStream salida(&archivo);
    for (int i = 0; i < historial->getCantidad(); i++) {
        Venta v = historial->getVenta(i);
        salida << "VENTA;" << v.getNumero() << ";" << v.getFecha() << ";" << v.getPago() << "\n";
        for (int j = 0; j < v.getCantidadLineas(); j++) {
            LineaVenta l = v.getLinea(j);
            salida << "LINEA;" << l.getCodigo() << ";" << l.getNombre() << ";"
                   << l.getCantidad() << ";" << l.getPrecio() << "\n";
        }
    }
    archivo.close();
    return true;
}

bool ArchivoVentas::cargar(Historial* historial)
{
    QFile archivo("ventas.txt");
    if (archivo.exists() == false) {
        return true;
    }
    if (archivo.open(QIODevice::ReadOnly) == false) {
        return false;
    }
    QTextStream entrada(&archivo);

    Venta ventaActual(0, "", 0);
    bool hayVenta = false;

    while (entrada.atEnd() == false) {
        QString linea = entrada.readLine();
        QStringList partes = linea.split(";");

        if (partes[0] == "VENTA" && partes.size() == 4) {
            // Antes de empezar una venta nueva se guarda la anterior
            if (hayVenta == true) {
                historial->agregar(ventaActual);
            }
            ventaActual = Venta(partes[1].toInt(), partes[2], partes[3].toInt());
            hayVenta = true;
        }
        if (partes[0] == "LINEA" && partes.size() == 5 && hayVenta == true) {
            LineaVenta l(partes[1], partes[2], partes[3].toInt(), partes[4].toInt());
            ventaActual.agregarLinea(l);
        }
    }
    // La última venta del archivo
    if (hayVenta == true) {
        historial->agregar(ventaActual);
    }
    archivo.close();
    return true;
}
