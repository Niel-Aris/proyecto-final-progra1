#include "ArchivoProductos.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>

bool ArchivoProductos::guardar(Inventario* inventario)
{
    QFile archivo("productos.txt");
    if (archivo.open(QIODevice::WriteOnly) == false) {
        return false;
    }
    QTextStream salida(&archivo);
    for (int i = 0; i < inventario->getCantidad(); i++) {
        Producto p = inventario->getProducto(i);
        QString activo = "1";
        if (p.getActivo() == false) {
            activo = "0";
        }
        salida << p.getCodigo() << ";" << p.getNombre() << ";" << p.getPrecio() << ";"
               << p.getExistencias() << ";" << activo << "\n";
    }
    archivo.close();
    return true;
}

bool ArchivoProductos::cargar(Inventario* inventario)
{
    QFile archivo("productos.txt");
    if (archivo.exists() == false) {
        return true;   // la primera vez no hay archivo y no es un error
    }
    if (archivo.open(QIODevice::ReadOnly) == false) {
        return false;
    }
    QTextStream entrada(&archivo);
    while (entrada.atEnd() == false) {
        QString linea = entrada.readLine();
        QStringList partes = linea.split(";");
        if (partes.size() == 5) {
            Producto p(partes[0], partes[1], partes[2].toInt(), partes[3].toInt());
            if (partes[4] == "0") {
                p.setActivo(false);
            }
            inventario->agregar(p);
        }
    }
    archivo.close();
    return true;
}
