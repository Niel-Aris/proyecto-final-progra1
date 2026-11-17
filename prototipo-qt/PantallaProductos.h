#pragma once
#include <QWidget>
#include <QStackedWidget>
#include <QString>

// Integrante 1: prototipo de la pantalla de productos.
// Son páginas fijas con datos de ejemplo, no guarda nada.
class PantallaProductos : public QWidget
{
    Q_OBJECT

public:
    PantallaProductos(QWidget* parent = nullptr);

private:
    QWidget* crearPagina(int numero, int filaElegida, QString estadoCafe, QString codigo,
                         QString nombre, QString precio, QString existencias, QString nota);

    void mostrarError();
    void elegirFila(int fila, int columna);
    void desactivar();
    void verLista();

    QStackedWidget* paginas;
};
