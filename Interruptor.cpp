#include "Interruptor.h"
#include "Estilo.h"
#include "Ajustes.h"
#include <QPainter>
#include <QColor>

Interruptor::Interruptor(QWidget* parent)
    : QAbstractButton(parent)
{
    setCheckable(true);
    setCursor(Qt::PointingHandCursor);
    posicion = 0.0;

    animacion = new QPropertyAnimation(this, "posicion", this);
    animacion->setDuration(220);
    animacion->setEasingCurve(QEasingCurve::OutCubic);

    connect(this, &QAbstractButton::toggled, this, &Interruptor::animar);
}

QSize Interruptor::sizeHint() const
{
    return QSize(46, 28);
}

double Interruptor::getPosicion() const
{
    return posicion;
}

void Interruptor::setPosicion(double posicion)
{
    this->posicion = posicion;
    update();
}

void Interruptor::animar(bool encendido)
{
    animacion->stop();
    if (Ajustes::animaciones() == false) {
        update();   // sin animación se dibuja de una vez en su lugar
        return;
    }
    animacion->setStartValue(posicion);
    if (encendido == true) {
        animacion->setEndValue(1.0);
    } else {
        animacion->setEndValue(0.0);
    }
    animacion->start();
}

void Interruptor::paintEvent(QPaintEvent* evento)
{
    // Si se cambió sin animación (por ejemplo al elegir otro producto), se dibuja donde corresponde
    double lugar = posicion;
    if (animacion->state() != QAbstractAnimation::Running) {
        if (isChecked() == true) {
            lugar = 1.0;
        } else {
            lugar = 0.0;
        }
        posicion = lugar;
    }

    QPainter pintor(this);
    pintor.setRenderHint(QPainter::Antialiasing);
    if (isEnabled() == false) {
        pintor.setOpacity(0.4);
    }

    // Fondo: de gris a verde según la posición
    QColor apagado = Estilo::color("interruptor");
    QColor encendido("#34c759");
    QColor fondo(apagado.red() + (encendido.red() - apagado.red()) * lugar,
                 apagado.green() + (encendido.green() - apagado.green()) * lugar,
                 apagado.blue() + (encendido.blue() - apagado.blue()) * lugar);
    QRectF pista(1, 1, width() - 2, height() - 2);
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(fondo);
    pintor.drawRoundedRect(pista, pista.height() / 2, pista.height() / 2);

    // Bolita blanca con una sombra suave
    double diametro = pista.height() - 4;
    double x = pista.left() + 2 + (pista.width() - diametro - 4) * lugar;
    double y = pista.top() + 2;
    pintor.setBrush(QColor(0, 0, 0, 30));
    pintor.drawEllipse(QRectF(x, y + 1.5, diametro, diametro));
    pintor.setBrush(QColor("#ffffff"));
    pintor.drawEllipse(QRectF(x, y, diametro, diametro));
}
