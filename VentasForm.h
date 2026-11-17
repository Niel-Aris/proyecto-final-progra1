#pragma once
#include <QWidget>
#include "ui_VentasForm.h"
#include "Carrito.h"
#include "Inventario.h"
#include "Historial.h"
#include "Venta.h"

// Integrante 2: pantalla de ventas en efectivo.
class VentasForm : public QWidget
{
    Q_OBJECT

public:
    VentasForm(Inventario* inventario, Historial* historial, QWidget* parent = nullptr);

private slots:
    void agregarAlCarrito();
    void cobrar();
    void cancelar();

private:
    void actualizarProductos();
    void actualizarTabla();
    void actualizarTotal();
    void guardarTicket(Venta venta);

    Ui::VentasForm ui;
    Inventario* inventario;
    Historial* historial;
    Carrito carrito;
};
