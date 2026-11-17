#include "TicketTxt.h"
#include "Ajustes.h"
#include <QLocale>

// El ticket mide 32 caracteres de ancho, como el papel de 58 mm de las impresoras de pulpería.
// Los nombres van a la izquierda y la plata siempre alineada a la derecha.
const int ANCHO = 32;

static QString colones(int monto)
{
    QLocale local(QLocale::Spanish, QLocale::CostaRica);
    return "₡" + local.toString(monto).replace(QChar(0x00A0), ' ').replace(QChar(0x202F), ' ');
}

// Una línea con texto a la izquierda y otro a la derecha, rellena con espacios en medio
static QString fila(QString izquierda, QString derecha)
{
    int espacios = ANCHO - izquierda.size() - derecha.size();
    if (espacios < 1) {
        espacios = 1;
    }
    return izquierda + QString(espacios, ' ') + derecha + "\n";
}

static QString centrado(QString texto)
{
    int espacios = (ANCHO - texto.size()) / 2;
    if (espacios < 0) {
        espacios = 0;
    }
    return QString(espacios, ' ') + texto + "\n";
}

QString TicketTxt::generar(Venta venta)
{
    QString texto;
    texto += centrado(("Pulpería " + Ajustes::nombreTienda()).toUpper());
    texto += centrado("Comprobante de venta");
    texto += QString(ANCHO, '=') + "\n";
    texto += fila("Venta #" + QString::number(venta.getNumero()), venta.getFecha());
    texto += QString(ANCHO, '-') + "\n";

    int unidades = 0;
    for (int i = 0; i < venta.getCantidadLineas(); i++) {
        LineaVenta l = venta.getLinea(i);
        unidades = unidades + l.getCantidad();
        texto += l.getNombre().left(ANCHO) + "\n";
        texto += fila("  " + QString::number(l.getCantidad()) + " x " + colones(l.getPrecio()), colones(l.getSubtotal()));
    }

    texto += QString(ANCHO, '-') + "\n";
    texto += fila("TOTAL", colones(venta.getTotal()));
    texto += fila("Pagó", colones(venta.getPago()));
    texto += fila("Vuelto", colones(venta.getCambio()));
    texto += QString(ANCHO, '=') + "\n";
    if (unidades == 1) {
        texto += centrado("1 artículo");
    } else {
        texto += centrado(QString::number(unidades) + " artículos");
    }
    texto += centrado("¡Gracias por su compra!");
    return texto;
}

QString TicketTxt::getExtension()
{
    return "txt";
}
