#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QString>

// Integrante 3: prototipo de la pantalla del historial.
// Son páginas fijas con una venta de ejemplo, no genera archivos.
class PantallaHistorial : public QWidget
{
    Q_OBJECT

public:
    PantallaHistorial(QWidget* parent = nullptr);

private:
    QWidget* crearPagina(int numero, QString formato, QString ticket, QString nota);

    void verTicket(int fila, int columna);
    void generarTicket();

    QStackedWidget* paginas;
};
