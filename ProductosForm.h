#pragma once
#include <QWidget>
#include "ui_ProductosForm.h"
#include "Inventario.h"

// Integrante 1: pantalla de productos
class ProductosForm : public QWidget
{
    Q_OBJECT

public:
    ProductosForm(Inventario* inventario, QWidget* parent = nullptr);
    void actualizar();

private:
    void seleccionarProducto(int fila, int columna);
    void agregarProducto();
    void modificarProducto();
    void cambiarEstado();
    void guardarArchivo();

    Ui::ProductosForm ui;
    Inventario* inventario;
};
