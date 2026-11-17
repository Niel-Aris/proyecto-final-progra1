#include "Iconos.h"
#include <QPainter>
#include <QPainterPath>
#include <QTransform>
#include <QPen>
#include <QLinearGradient>
#include <QFont>
#include <QGuiApplication>
#include <QStringList>
#include <cmath>

// Todos los íconos se dibujan en 40 x 40 (20 puntos al doble para pantallas Retina)
static QPixmap lienzo()
{
    QPixmap imagen(40, 40);
    imagen.fill(Qt::transparent);
    return imagen;
}

static QIcon terminar(QPixmap imagen)
{
    imagen.setDevicePixelRatio(2.0);
    return QIcon(imagen);
}

QIcon Iconos::luna(QColor color)
{
    QPixmap imagen = lienzo();
    QPainter pintor(&imagen);
    pintor.setRenderHint(QPainter::Antialiasing);
    QPainterPath circulo;
    circulo.addEllipse(QPointF(20, 20), 13, 13);
    QPainterPath mordida;
    mordida.addEllipse(QPointF(28, 13), 11, 11);
    pintor.fillPath(circulo.subtracted(mordida), color);
    pintor.end();
    return terminar(imagen);
}

QIcon Iconos::sol(QColor color)
{
    QPixmap imagen = lienzo();
    QPainter pintor(&imagen);
    pintor.setRenderHint(QPainter::Antialiasing);
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(color);
    pintor.drawEllipse(QPointF(20, 20), 7, 7);
    pintor.setPen(QPen(color, 3, Qt::SolidLine, Qt::RoundCap));
    for (int i = 0; i < 8; i++) {
        double angulo = i * 3.14159265 / 4;
        pintor.drawLine(QPointF(20 + std::cos(angulo) * 11, 20 + std::sin(angulo) * 11),
                        QPointF(20 + std::cos(angulo) * 16, 20 + std::sin(angulo) * 16));
    }
    pintor.end();
    return terminar(imagen);
}

QIcon Iconos::engrane(QColor color)
{
    QPixmap imagen = lienzo();
    QPainter pintor(&imagen);
    pintor.setRenderHint(QPainter::Antialiasing);
    pintor.translate(20, 20);
    QPainterPath engrane;
    engrane.addEllipse(QPointF(0, 0), 12, 12);
    for (int i = 0; i < 8; i++) {
        QPainterPath diente;
        diente.addRoundedRect(QRectF(-3.5, -17, 7, 8), 2, 2);
        QTransform giro;
        giro.rotate(i * 45);
        engrane = engrane.united(giro.map(diente));
    }
    QPainterPath hueco;
    hueco.addEllipse(QPointF(0, 0), 5, 5);
    pintor.fillPath(engrane.subtracted(hueco), color);
    pintor.end();
    return terminar(imagen);
}

QPixmap Iconos::avatar(QString nombre, int lado)
{
    int pixeles = lado * 2;
    QPixmap imagen(pixeles, pixeles);
    imagen.fill(Qt::transparent);
    QPainter pintor(&imagen);
    pintor.setRenderHint(QPainter::Antialiasing);

    // Cada nombre tiene su color
    int tono = qHash(nombre) % 360;
    QLinearGradient degradado(0, 0, 0, pixeles);
    degradado.setColorAt(0, QColor::fromHsl(tono, 150, 150));
    degradado.setColorAt(1, QColor::fromHsl(tono, 160, 110));
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(degradado);
    pintor.drawEllipse(QRectF(0, 0, pixeles, pixeles));

    QString iniciales;
    QStringList palabras = nombre.split(" ", Qt::SkipEmptyParts);
    for (int i = 0; i < palabras.size() && iniciales.size() < 2; i++) {
        iniciales += palabras[i][0].toUpper();
    }
    QFont letra = QGuiApplication::font();
    letra.setPixelSize(pixeles * 0.42);
    letra.setBold(true);
    pintor.setFont(letra);
    pintor.setPen(QColor("#ffffff"));
    pintor.drawText(QRectF(0, 0, pixeles, pixeles), Qt::AlignCenter, iniciales);
    pintor.end();
    imagen.setDevicePixelRatio(2.0);
    return imagen;
}
