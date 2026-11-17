#pragma once
#include <QAbstractButton>
#include <QPropertyAnimation>

// Interruptor de encendido y apagado como el de iPhone.
// Funciona como un botón con checkable: isChecked() dice si está encendido.
class Interruptor : public QAbstractButton
{
    Q_OBJECT
    Q_PROPERTY(double posicion READ getPosicion WRITE setPosicion)

public:
    Interruptor(QWidget* parent = nullptr);
    QSize sizeHint() const override;

    double getPosicion() const;
    void setPosicion(double posicion);

protected:
    void paintEvent(QPaintEvent* evento) override;

private:
    void animar(bool encendido);

    double posicion;   // 0 = apagado, 1 = encendido; en medio mientras se mueve
    QPropertyAnimation* animacion;
};
