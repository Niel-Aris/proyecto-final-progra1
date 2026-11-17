#pragma once
#include <QPixmap>
#include <QString>

// Imágenes de los productos, para usarlas en Productos y en Vender.
// Las de los productos de ejemplo están en imagenes/ con el código como nombre (P001.png...).
// Si un producto no tiene imagen, se dibuja una tarjeta con sus iniciales.
// Los métodos son static: se usan así, Imagenes::producto(codigo, nombre, activo)
class Imagenes
{
public:
    // Tarjeta completa de 760 x 340
    static QPixmap producto(QString codigo, QString nombre, bool activo);

    // Cuadrito con el centro de la imagen y esquinas redondeadas, de "lado" puntos
    static QPixmap cuadrada(QString codigo, QString nombre, bool activo, int lado);
};
