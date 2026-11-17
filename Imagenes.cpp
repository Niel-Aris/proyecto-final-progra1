#include "Imagenes.h"
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QGuiApplication>
#include <QStringList>
#include <QColor>
#include <QBrush>

QPixmap Imagenes::producto(QString codigo, QString nombre, bool activo)
{
    QPixmap imagen(":/imagenes/" + codigo + ".png");

    if (imagen.isNull() == true) {
        // Tarjeta del mismo tamaño que las imágenes (760 x 340), con un color que sale del nombre
        imagen = QPixmap(760, 340);
        imagen.fill(Qt::transparent);
        int tono = qHash(nombre) % 360;

        QPainter pintor(&imagen);
        pintor.setRenderHint(QPainter::Antialiasing);
        QLinearGradient degradado(0, 0, 760, 340);
        degradado.setColorAt(0, QColor::fromHsl(tono, 160, 238));
        degradado.setColorAt(1, QColor::fromHsl(tono, 130, 205));
        pintor.setPen(Qt::NoPen);
        pintor.setBrush(degradado);
        pintor.drawRoundedRect(QRectF(0, 0, 760, 340), 28, 28);
        pintor.setBrush(QColor(255, 255, 255, 70));
        pintor.drawEllipse(QPointF(650, 50), 110, 110);
        pintor.drawEllipse(QPointF(90, 310), 80, 80);

        // Solo cuentan las palabras de más de 2 letras, así "Azúcar 1 kg" da "AZ" y no "AK"
        QString iniciales;
        QStringList palabras;
        QStringList todas = nombre.split(" ", Qt::SkipEmptyParts);
        for (int i = 0; i < todas.size(); i++) {
            if (todas[i].size() > 2 && todas[i][0].isLetter() == true) {
                palabras.append(todas[i]);
            }
        }
        if (palabras.size() == 1) {
            iniciales = palabras[0].left(2).toUpper();
        } else if (palabras.size() >= 2) {
            iniciales = QString(palabras[0][0].toUpper()) + palabras[1][0].toUpper();
        }
        if (iniciales == "") {
            iniciales = "+";
        }
        pintor.setBrush(QColor(255, 255, 255, 215));
        pintor.drawEllipse(QPointF(380, 170), 92, 92);
        QFont letra = QGuiApplication::font();
        letra.setPixelSize(70);
        letra.setBold(true);
        pintor.setFont(letra);
        pintor.setPen(QColor::fromHsl(tono, 110, 85));
        pintor.drawText(QRectF(288, 78, 184, 184), Qt::AlignCenter, iniciales);
        pintor.end();
    }

    // Un producto inactivo se ve deslavado
    if (activo == false) {
        QPainter pintor(&imagen);
        pintor.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        pintor.fillRect(imagen.rect(), QColor(245, 245, 247, 150));
        pintor.end();
    }
    return imagen;
}

QPixmap Imagenes::cuadrada(QString codigo, QString nombre, bool activo, int lado)
{
    // Se dibuja al doble para que se vea nítido en pantallas Retina
    int pixeles = lado * 2;
    QPixmap imagen = producto(codigo, nombre, activo);
    QPixmap centro = imagen.copy(230, 20, 300, 300).scaled(pixeles, pixeles, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap redondeada(pixeles, pixeles);
    redondeada.fill(Qt::transparent);
    QPainter pintor(&redondeada);
    pintor.setRenderHint(QPainter::Antialiasing);
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QBrush(centro));
    double radio = pixeles * 0.22;
    pintor.drawRoundedRect(QRectF(0, 0, pixeles, pixeles), radio, radio);
    pintor.end();
    redondeada.setDevicePixelRatio(2.0);
    return redondeada;
}
