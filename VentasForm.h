#pragma once
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QPointer>
#include <QVariantAnimation>
#include <QResizeEvent>
#include "ui_VentasForm.h"
#include "Carrito.h"
#include "Inventario.h"
#include "Historial.h"
#include "Venta.h"

// Integrante 2: pantalla de ventas en efectivo.
// A la izquierda los productos como tarjetas con su imagen: un clic los agrega.
// A la derecha la venta actual, el total, con cuánto paga y el vuelto.
class VentasForm : public QWidget
{
    Q_OBJECT

public:
    VentasForm(Inventario* inventario, Historial* historial, QWidget* parent = nullptr);
    void actualizar();   // la llama MainWindow al cambiar de pestaña

private:
    // Catálogo
    void llenarCatalogo();
    void filtrarCatalogo();
    void agregarDesdeCatalogo(QListWidgetItem* item);
    void agregarPrimero();
    void animarVuelo(QListWidgetItem* item);

    // Carrito
    void agregarProducto(QString codigo);
    void cambiarCantidad(int posicion, int cantidad);
    void quitarLinea(int posicion);
    void vaciar();
    void deshacer();
    void mostrarCarrito(int resaltar);
    QWidget* crearFila(int posicion);

    // Cobro
    void mostrarTotal();
    void mostrarPago();
    void ponerPago(int monto);
    void cobrar();
    void mostrarExito(Venta venta);
    void cerrarExito();

    // Avisos
    void mostrarError(QString error);
    void mostrarBarra(QString texto);
    QString colones(int monto) const;
    void resizeEvent(QResizeEvent* evento) override;

    Ui::VentasForm ui;
    Inventario* inventario;
    Historial* historial;
    Carrito carrito;
    Carrito carritoAnterior;     // para deshacer al vaciar o quitar una línea
    int totalMostrado;           // el total que se ve, para animar el número
    QTimer timerBarra;
    QTimer timerError;
    QTimer timerExito;
    QPointer<QVariantAnimation> animacionTotal;
    QPushButton* rapidos[4];     // botones de pago rápido
    int montosRapidos[4];

    // Tarjeta de "venta lista" que aparece encima del carrito
    QFrame* frmExito;
    QLabel* lblCheque;
    QLabel* lblExitoTitulo;
    QLabel* lblExitoVuelto;
    QLabel* lblExitoDetalle;
    QPushButton* btnNuevaVenta;
};
