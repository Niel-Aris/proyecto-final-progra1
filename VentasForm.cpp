#include "VentasForm.h"
#include <QMessageBox>
#include <QTableWidgetItem>
#include "ArchivoProductos.h"
#include "ArchivoVentas.h"
#include "TicketTxt.h"

VentasForm::VentasForm(Inventario* inventario, Historial* historial, QWidget* parent)
    : QWidget(parent)
{
    this->inventario = inventario;
    this->historial = historial;

    ui.setupUi(this);

    connect(ui.btnAgregar, &QPushButton::clicked, this, &VentasForm::agregarAlCarrito);
    connect(ui.btnCobrar, &QPushButton::clicked, this, &VentasForm::cobrar);
    connect(ui.btnCancelar, &QPushButton::clicked, this, &VentasForm::cancelar);

    actualizarProductos();
    actualizarTabla();
    actualizarTotal();
}

// MainWindow la llama al cambiar de pestaña, por si cambiaron los productos
void VentasForm::actualizar()
{
    actualizarProductos();
}

void VentasForm::actualizarProductos()
{
    ui.cmbProductos->clear();

    for (int i = 0; i < inventario->getCantidad(); i++) {
        Producto producto = inventario->getProducto(i);
        if (producto.getActivo() == true && producto.getExistencias() > 0) {
            QString texto = producto.getNombre() + " (₡" + QString::number(producto.getPrecio()) + ")";
            ui.cmbProductos->addItem(texto, producto.getCodigo());
        }
    }
}

void VentasForm::actualizarTabla()
{
    ui.tablaCarrito->setRowCount(carrito.getCantidad());

    for (int i = 0; i < carrito.getCantidad(); i++) {
        LineaVenta linea = carrito.getLinea(i);
        ui.tablaCarrito->setItem(i, 0, new QTableWidgetItem(linea.getNombre()));
        ui.tablaCarrito->setItem(i, 1, new QTableWidgetItem(QString::number(linea.getCantidad())));
        ui.tablaCarrito->setItem(i, 2, new QTableWidgetItem("₡" + QString::number(linea.getPrecio())));
        ui.tablaCarrito->setItem(i, 3, new QTableWidgetItem("₡" + QString::number(linea.getSubtotal())));
    }
}

void VentasForm::actualizarTotal()
{
    ui.lblTotal->setText("Total: ₡" + QString::number(carrito.getTotal()));
}

void VentasForm::agregarAlCarrito()
{
    if (ui.cmbProductos->currentIndex() == -1) {
        return;
    }

    QString codigo = ui.cmbProductos->currentData().toString();
    int posicion = inventario->buscar(codigo);
    Producto producto = inventario->getProducto(posicion);

    QString error = carrito.agregar(producto, ui.spnCantidad->value());
    if (error != "") {
        QMessageBox::warning(this, "No se pudo agregar", error);
        return;
    }

    actualizarTabla();
    actualizarTotal();
}

void VentasForm::cobrar()
{
    QString error = carrito.cobrar(ui.spnPago->value(), inventario, historial);
    if (error != "") {
        QMessageBox::warning(this, "No se pudo cobrar", error);
        return;
    }

    Venta venta = historial->getVenta(historial->getCantidad() - 1);

    // El inventario ya bajó: se guarda para que no se pierda al cerrar
    if (ArchivoProductos::guardar(inventario) == false) {
        QMessageBox::critical(this, "Error", "No se pudo guardar productos.txt");
    }

    if (ArchivoVentas::guardar(historial) == false) {
        QMessageBox::critical(this, "Error", "No se pudo guardar ventas.txt");
    }

    TicketTxt ticket;
    if (ticket.guardar(venta) == false) {
        QMessageBox::warning(this, "Ticket", "No se pudo guardar el ticket.");
    }

    QMessageBox::information(this, "Venta realizada",
        "Venta #" + QString::number(venta.getNumero()) +
        "\nCambio: ₡" + QString::number(venta.getCambio()));

    ui.spnPago->setValue(0);
    ui.spnCantidad->setValue(1);
    actualizarProductos();
    actualizarTabla();
    actualizarTotal();
}

void VentasForm::cancelar()
{
    carrito.vaciar();
    ui.spnPago->setValue(0);
    ui.spnCantidad->setValue(1);
    actualizarTabla();
    actualizarTotal();
}

