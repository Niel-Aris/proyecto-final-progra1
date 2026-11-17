#pragma once
#include <QWidget>
#include <QPropertyAnimation>
#include "Venta.h"

// Dibuja el ticket de una venta como un recibo de papel de verdad:
// papel blanco con sombra, borde de abajo en zigzag, líneas punteadas y un código de barras.
// Al elegir otra venta el recibo "sale de la impresora" con una animación.
class VistaTicket : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double avance READ getAvance WRITE setAvance)

public:
    VistaTicket(QWidget* parent = nullptr);

    void mostrarVenta(Venta venta, bool animar);
    void limpiar();

    double getAvance() const;
    void setAvance(double avance);

protected:
    void paintEvent(QPaintEvent* evento) override;

private:
    int altoPapel() const;

    Venta venta;
    bool hayVenta;
    double avance;     // 0 = el papel todavía no sale, 1 = recibo completo
    QPropertyAnimation* animacion;
};
