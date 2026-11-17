#include "ArchivoProductos.h"
#include <QFile>
#include <QSaveFile>
#include <QTextStream>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

// Escribe un JSON en un archivo. QSaveFile escribe primero en un archivo temporal
// y solo al final lo cambia por el real, así un corte a medias no deja el archivo dañado.
static bool escribirJson(QString nombre, QJsonArray datos)
{
    QSaveFile archivo(nombre);
    if (archivo.open(QIODevice::WriteOnly) == false) {
        return false;
    }
    archivo.write(QJsonDocument(datos).toJson(QJsonDocument::Indented));
    return archivo.commit();
}

bool ArchivoProductos::guardar(Inventario* inventario)
{
    QJsonArray productos;
    for (int i = 0; i < inventario->getCantidad(); i++) {
        Producto p = inventario->getProducto(i);
        QJsonObject objeto;
        objeto["codigo"] = p.getCodigo();
        objeto["nombre"] = p.getNombre();
        objeto["precio"] = p.getPrecio();
        objeto["existencias"] = p.getExistencias();
        objeto["activo"] = p.getActivo();
        productos.append(objeto);
    }

    // La actividad de cada producto va en otro archivo
    QJsonArray movimientos;
    for (int i = 0; i < inventario->getCantidadMovimientos(); i++) {
        Movimiento m = inventario->getMovimiento(i);
        QJsonObject objeto;
        objeto["fecha"] = m.getFecha();
        objeto["codigo"] = m.getCodigo();
        objeto["texto"] = m.getTexto();
        movimientos.append(objeto);
    }

    if (escribirJson("productos.json", productos) == false) {
        return false;
    }
    return escribirJson("movimientos.json", movimientos);
}

bool ArchivoProductos::cargar(Inventario* inventario)
{
    QFile archivo("productos.json");
    if (archivo.exists() == false) {
        return cargarTxt(inventario);   // datos de antes o primera vez
    }
    if (archivo.open(QIODevice::ReadOnly) == false) {
        return false;
    }
    QJsonParseError error;
    QJsonDocument documento = QJsonDocument::fromJson(archivo.readAll(), &error);
    archivo.close();
    if (error.error != QJsonParseError::NoError || documento.isArray() == false) {
        // El archivo está dañado: se guarda una copia para no perderlo al siguiente guardado
        QFile::remove("productos-danado.json");
        QFile::copy("productos.json", "productos-danado.json");
        return false;   // MainWindow avisa
    }

    QJsonArray productos = documento.array();
    for (int i = 0; i < productos.size(); i++) {
        QJsonObject objeto = productos[i].toObject();
        Producto p(objeto["codigo"].toString(), objeto["nombre"].toString(),
                   objeto["precio"].toInt(), objeto["existencias"].toInt());
        p.setActivo(objeto["activo"].toBool(true));
        inventario->agregar(p);
    }

    // Cargar los productos no cuenta como cambios: se borra eso y se lee la actividad guardada
    inventario->limpiarHistorial();
    QFile archivoMovimientos("movimientos.json");
    if (archivoMovimientos.open(QIODevice::ReadOnly) == true) {
        QJsonArray movimientos = QJsonDocument::fromJson(archivoMovimientos.readAll()).array();
        archivoMovimientos.close();
        for (int i = 0; i < movimientos.size(); i++) {
            QJsonObject objeto = movimientos[i].toObject();
            inventario->agregarMovimiento(Movimiento(objeto["fecha"].toString(), objeto["codigo"].toString(),
                                                     objeto["texto"].toString()));
        }
    }
    return true;
}

// Lee el formato viejo (productos.txt y movimientos.txt, separados por ;)
bool ArchivoProductos::cargarTxt(Inventario* inventario)
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
        QStringList partes = entrada.readLine().split(";");
        if (partes.size() == 5) {
            Producto p(partes[0], partes[1], partes[2].toInt(), partes[3].toInt());
            if (partes[4] == "0") {
                p.setActivo(false);
            }
            inventario->agregar(p);
        }
    }
    archivo.close();

    inventario->limpiarHistorial();
    QFile archivoMovimientos("movimientos.txt");
    if (archivoMovimientos.open(QIODevice::ReadOnly) == true) {
        QTextStream entradaMovimientos(&archivoMovimientos);
        while (entradaMovimientos.atEnd() == false) {
            QStringList partes = entradaMovimientos.readLine().split(";");
            if (partes.size() == 3) {
                inventario->agregarMovimiento(Movimiento(partes[0], partes[1], partes[2]));
            }
        }
        archivoMovimientos.close();
    }
    return true;
}
