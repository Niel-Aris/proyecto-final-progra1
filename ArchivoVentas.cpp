#include "ArchivoVentas.h"
#include <QFile>
#include <QSaveFile>
#include <QTextStream>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

bool ArchivoVentas::guardar(Historial* historial)
{
    QJsonArray ventas;
    for (int i = 0; i < historial->getCantidad(); i++) {
        Venta v = historial->getVenta(i);
        QJsonArray lineas;
        for (int j = 0; j < v.getCantidadLineas(); j++) {
            LineaVenta l = v.getLinea(j);
            QJsonObject linea;
            linea["codigo"] = l.getCodigo();
            linea["nombre"] = l.getNombre();
            linea["cantidad"] = l.getCantidad();
            linea["precio"] = l.getPrecio();
            lineas.append(linea);
        }
        QJsonObject venta;
        venta["numero"] = v.getNumero();
        venta["fecha"] = v.getFecha();
        venta["pago"] = v.getPago();
        venta["lineas"] = lineas;
        ventas.append(venta);
    }

    // QSaveFile escribe en un temporal y lo cambia al final: un corte a medias no daña el archivo
    QSaveFile archivo("ventas.json");
    if (archivo.open(QIODevice::WriteOnly) == false) {
        return false;
    }
    archivo.write(QJsonDocument(ventas).toJson(QJsonDocument::Indented));
    return archivo.commit();
}

bool ArchivoVentas::cargar(Historial* historial)
{
    QFile archivo("ventas.json");
    if (archivo.exists() == false) {
        return cargarTxt(historial);   // datos de antes o primera vez
    }
    if (archivo.open(QIODevice::ReadOnly) == false) {
        return false;
    }
    QJsonParseError error;
    QJsonDocument documento = QJsonDocument::fromJson(archivo.readAll(), &error);
    archivo.close();
    if (error.error != QJsonParseError::NoError || documento.isArray() == false) {
        // El archivo está dañado: se guarda una copia para no perderlo al siguiente guardado
        QFile::remove("ventas-danado.json");
        QFile::copy("ventas.json", "ventas-danado.json");
        return false;   // MainWindow avisa
    }

    QJsonArray ventas = documento.array();
    for (int i = 0; i < ventas.size(); i++) {
        QJsonObject objeto = ventas[i].toObject();
        Venta venta(objeto["numero"].toInt(), objeto["fecha"].toString(), objeto["pago"].toInt());
        QJsonArray lineas = objeto["lineas"].toArray();
        for (int j = 0; j < lineas.size(); j++) {
            QJsonObject l = lineas[j].toObject();
            venta.agregarLinea(LineaVenta(l["codigo"].toString(), l["nombre"].toString(),
                                          l["cantidad"].toInt(), l["precio"].toInt()));
        }
        historial->agregar(venta);
    }
    return true;
}

// Lee el formato viejo de ventas.txt:
// VENTA;numero;fecha;pago y debajo una línea LINEA;codigo;nombre;cantidad;precio por producto
bool ArchivoVentas::cargarTxt(Historial* historial)
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
        QStringList partes = entrada.readLine().split(";");

        if (partes[0] == "VENTA" && partes.size() == 4) {
            // Antes de empezar una venta nueva se guarda la anterior
            if (hayVenta == true) {
                historial->agregar(ventaActual);
            }
            ventaActual = Venta(partes[1].toInt(), partes[2], partes[3].toInt());
            hayVenta = true;
        }
        if (partes[0] == "LINEA" && partes.size() == 5 && hayVenta == true) {
            ventaActual.agregarLinea(LineaVenta(partes[1], partes[2], partes[3].toInt(), partes[4].toInt()));
        }
    }
    // La última venta del archivo
    if (hayVenta == true) {
        historial->agregar(ventaActual);
    }
    archivo.close();
    return true;
}
