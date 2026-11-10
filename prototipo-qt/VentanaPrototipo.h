#pragma once
#include <QMainWindow>
#include <QTabWidget>

// Integrante 1: ventana principal del prototipo, con las pestañas
class VentanaPrototipo : public QMainWindow
{
    Q_OBJECT

public:
    VentanaPrototipo(QWidget* parent = nullptr);

private:
    QWidget* crearEstilos();

    QTabWidget* pestanas;
};
