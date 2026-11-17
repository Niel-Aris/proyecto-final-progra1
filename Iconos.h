#pragma once
#include <QIcon>
#include <QPixmap>
#include <QColor>
#include <QString>

// Íconos dibujados con código, así se ven nítidos y cambian de color con el tema.
// Los métodos son static: se usan así, Iconos::luna(color)
class Iconos
{
public:
    static QIcon luna(QColor color);
    static QIcon sol(QColor color);
    static QIcon engrane(QColor color);

    // Círculo de color con las iniciales del usuario, como en la pantalla de inicio de macOS
    static QPixmap avatar(QString nombre, int lado);
};
