#include "VistaTicket.h"
#include "Ajustes.h"
#include <QPainter>
#include <QPainterPath>
#include <QLocale>
#include <QFont>
#include <QPen>
#include <QColor>
#include <QStringList>

// Medidas del recibo, en puntos
const int ANCHO_PAPEL = 300;
const int MARGEN = 22;
const int ALTO_ENCABEZADO = 118;
const int ALTO_LINEA = 42;
const int ALTO_PIE = 236;
const int DIENTE = 10;   // tamaño del zigzag de abajo

static QString colones(int monto)
{
    QLocale local(QLocale::Spanish, QLocale::CostaRica);
    return "₡" + local.toString(monto);
}

VistaTicket::VistaTicket(QWidget* parent)
    : QWidget(parent), venta(0, "", 0)
{
    hayVenta = false;
    avance = 1.0;
    animacion = new QPropertyAnimation(this, "avance", this);
    animacion->setDuration(650);
    animacion->setEasingCurve(QEasingCurve::OutCubic);
    setMinimumSize(ANCHO_PAPEL + 60, 200);
}

double VistaTicket::getAvance() const
{
    return avance;
}

void VistaTicket::setAvance(double avance)
{
    this->avance = avance;
    update();
}

int VistaTicket::altoPapel() const
{
    return ALTO_ENCABEZADO + venta.getCantidadLineas() * ALTO_LINEA + ALTO_PIE;
}

void VistaTicket::mostrarVenta(Venta venta, bool animar)
{
    this->venta = venta;
    hayVenta = true;
    setMinimumHeight(altoPapel() + 40);
    animacion->stop();
    if (animar == true && Ajustes::animaciones() == true) {
        animacion->setStartValue(0.0);
        animacion->setEndValue(1.0);
        animacion->start();
    } else {
        avance = 1.0;
    }
    update();
}

void VistaTicket::limpiar()
{
    hayVenta = false;
    animacion->stop();
    setMinimumHeight(200);
    update();
}

void VistaTicket::paintEvent(QPaintEvent* evento)
{
    QPainter pintor(this);
    pintor.setRenderHint(QPainter::Antialiasing);
    pintor.setRenderHint(QPainter::TextAntialiasing);

    if (hayVenta == false) {
        pintor.setPen(QColor("#8e8e93"));
        pintor.drawText(rect(), Qt::AlignCenter, "Elige una venta para ver su ticket.");
        return;
    }

    int alto = altoPapel();
    double x0 = (width() - ANCHO_PAPEL) / 2.0;
    double y0 = 16;

    // El papel sale de arriba hacia abajo: se recorta lo que todavía no "salió"
    double visible = alto * avance;
    pintor.setClipRect(QRectF(0, y0 - 4, width(), visible + 24));
    pintor.translate(0, visible - alto);

    // Forma del papel: arriba recto, abajo en zigzag
    QPainterPath papel;
    papel.moveTo(x0, y0);
    papel.lineTo(x0 + ANCHO_PAPEL, y0);
    papel.lineTo(x0 + ANCHO_PAPEL, y0 + alto);
    int dientes = ANCHO_PAPEL / (DIENTE * 2);
    double paso = ANCHO_PAPEL / (double)dientes;
    for (int i = 0; i < dientes; i++) {
        double derecha = x0 + ANCHO_PAPEL - i * paso;
        papel.lineTo(derecha - paso / 2, y0 + alto + DIENTE);
        papel.lineTo(derecha - paso, y0 + alto);
    }
    papel.closeSubpath();

    // Sombra suave debajo del papel
    for (int i = 6; i >= 1; i--) {
        pintor.setPen(Qt::NoPen);
        pintor.setBrush(QColor(0, 0, 0, 5));
        pintor.drawPath(papel.translated(0, i * 1.2));
    }
    pintor.setBrush(QColor("#ffffff"));
    pintor.setPen(QPen(QColor("#e6e6ea"), 1));
    pintor.drawPath(papel);

    QFont normal = font();
    normal.setPixelSize(13);
    QFont chica = font();
    chica.setPixelSize(11);
    QFont negrita = normal;
    negrita.setBold(true);
    QFont titulo = font();
    titulo.setPixelSize(20);
    titulo.setBold(true);
    QFont grande = font();
    grande.setPixelSize(22);
    grande.setBold(true);
    QFont mono = QFont("Menlo");
    mono.setStyleHint(QFont::Monospace);
    mono.setPixelSize(11);

    QColor tinta("#1d1d1f");
    QColor gris("#8e8e93");
    double izquierda = x0 + MARGEN;
    double derecha = x0 + ANCHO_PAPEL - MARGEN;
    double anchoTexto = derecha - izquierda;
    QPen punteada(QColor("#c7c7cc"), 1, Qt::DashLine);
    punteada.setDashPattern({3, 3});

    // Encabezado: sello con las iniciales y el nombre de la pulpería
    double y = y0 + 22;
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor("#0071e3"));
    pintor.drawEllipse(QPointF(x0 + ANCHO_PAPEL / 2.0, y + 14), 15, 15);
    pintor.setPen(QColor("#ffffff"));
    pintor.setFont(negrita);
    // Sello con las iniciales de la pulpería (La Pulpe = LP)
    QString iniciales;
    QStringList palabras = Ajustes::nombreTienda().split(" ", Qt::SkipEmptyParts);
    for (int i = 0; i < palabras.size() && iniciales.size() < 2; i++) {
        iniciales += palabras[i][0].toUpper();
    }
    pintor.drawText(QRectF(x0 + ANCHO_PAPEL / 2.0 - 15, y - 1, 30, 30), Qt::AlignCenter, iniciales);
    y = y + 34;
    pintor.setPen(tinta);
    pintor.setFont(titulo);
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 24), Qt::AlignCenter, "Pulpería " + Ajustes::nombreTienda());
    y = y + 24;
    pintor.setFont(chica);
    pintor.setPen(gris);
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 16), Qt::AlignCenter,
                    "Venta #" + QString::number(venta.getNumero()) + "   ·   " + venta.getFecha());
    y = y + 28;
    pintor.setPen(punteada);
    pintor.drawLine(QPointF(izquierda, y), QPointF(derecha, y));

    // Productos: nombre y subtotal en una línea, cantidad por precio debajo
    y = y + 8;
    int unidades = 0;
    for (int i = 0; i < venta.getCantidadLineas(); i++) {
        LineaVenta l = venta.getLinea(i);
        unidades = unidades + l.getCantidad();
        pintor.setFont(negrita);
        pintor.setPen(tinta);
        QString nombre = pintor.fontMetrics().elidedText(l.getNombre(), Qt::ElideRight, anchoTexto - 90);
        pintor.drawText(QRectF(izquierda, y, anchoTexto, 18), Qt::AlignLeft | Qt::AlignVCenter, nombre);
        pintor.drawText(QRectF(izquierda, y, anchoTexto, 18), Qt::AlignRight | Qt::AlignVCenter, colones(l.getSubtotal()));
        pintor.setFont(chica);
        pintor.setPen(gris);
        pintor.drawText(QRectF(izquierda, y + 17, anchoTexto, 16), Qt::AlignLeft | Qt::AlignVCenter,
                        QString::number(l.getCantidad()) + " × " + colones(l.getPrecio()));
        y = y + ALTO_LINEA;
    }

    pintor.setPen(punteada);
    pintor.drawLine(QPointF(izquierda, y), QPointF(derecha, y));

    // Totales
    y = y + 12;
    pintor.setPen(tinta);
    pintor.setFont(negrita);
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 28), Qt::AlignLeft | Qt::AlignVCenter, "Total");
    pintor.setFont(grande);
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 28), Qt::AlignRight | Qt::AlignVCenter, colones(venta.getTotal()));
    y = y + 34;
    pintor.setFont(normal);
    pintor.setPen(gris);
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 18), Qt::AlignLeft | Qt::AlignVCenter, "Pagó en efectivo");
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 18), Qt::AlignRight | Qt::AlignVCenter, colones(venta.getPago()));
    y = y + 22;
    pintor.setPen(QColor("#1a7f37"));
    pintor.setFont(negrita);
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 18), Qt::AlignLeft | Qt::AlignVCenter, "Vuelto");
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 18), Qt::AlignRight | Qt::AlignVCenter, colones(venta.getCambio()));
    y = y + 30;
    pintor.setPen(punteada);
    pintor.drawLine(QPointF(izquierda, y), QPointF(derecha, y));

    // Pie: artículos, gracias y un código de barras hecho con el número de venta
    y = y + 12;
    pintor.setFont(chica);
    pintor.setPen(gris);
    QString articulos = QString::number(unidades) + " artículos";
    if (unidades == 1) {
        articulos = "1 artículo";
    }
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 16), Qt::AlignCenter, articulos + "   ·   ¡Gracias por su compra!");
    y = y + 26;
    QString codigo = QString::number(venta.getNumero()).rightJustified(8, '0');
    double xBarra = x0 + ANCHO_PAPEL / 2.0 - 84;
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(tinta);
    for (int i = 0; i < codigo.size() * 3; i++) {
        int digito = codigo[i / 3].digitValue();
        double grosor = 1 + ((digito + i) % 3);
        if ((digito + i) % 2 == 0) {
            pintor.drawRect(QRectF(xBarra, y, grosor, 36));
        }
        xBarra = xBarra + 7;
    }
    y = y + 40;
    pintor.setFont(mono);
    pintor.setPen(gris);
    pintor.drawText(QRectF(izquierda, y, anchoTexto, 14), Qt::AlignCenter, codigo);
}
