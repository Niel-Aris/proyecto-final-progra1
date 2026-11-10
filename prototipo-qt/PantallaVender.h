#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QTabWidget>
#include <QString>

// Integrante 2: prototipo de la pantalla de ventas.
// Son páginas fijas con datos de ejemplo, no calcula nada.
class PantallaVender : public QWidget
{
    Q_OBJECT

public:
    PantallaVender(QTabWidget* pestanas, QWidget* parent = nullptr);

private:
    QWidget* crearPagina(int numero, QString producto, QString cantidad, bool conLineas,
                         QString total, QString pago, QString nota);

    void verCarrito();
    void cobrar();
    void cancelar();
    void verSinExistencias();
    void verPagoInsuficiente();

    QStackedWidget* paginas;
    QTabWidget* pestanas;
};
