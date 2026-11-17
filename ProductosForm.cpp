#include "ProductosForm.h"
#include "ArchivoProductos.h"
#include <QMessageBox>

ProductosForm::ProductosForm(Inventario* inventario, QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->inventario = inventario;

    // cellClicked cubre el clic y currentCellChanged el cambio de fila con el teclado
    connect(ui.tablaProductos, &QTableWidget::cellClicked, this, &ProductosForm::seleccionarProducto);
    connect(ui.tablaProductos, &QTableWidget::currentCellChanged, this, &ProductosForm::seleccionarProducto);
    connect(ui.btnAgregar, &QPushButton::clicked, this, &ProductosForm::agregarProducto);
    connect(ui.btnModificar, &QPushButton::clicked, this, &ProductosForm::modificarProducto);
    connect(ui.btnDesactivar, &QPushButton::clicked, this, &ProductosForm::cambiarEstado);

    actualizar();
}

// Vuelve a llenar la tabla con los productos del inventario
void ProductosForm::actualizar()
{
    ui.tablaProductos->setRowCount(inventario->getCantidad());
    for (int i = 0; i < inventario->getCantidad(); i++) {
        Producto p = inventario->getProducto(i);
        QString estado = "Activo";
        if (p.getActivo() == false) {
            estado = "Inactivo";
        }
        ui.tablaProductos->setItem(i, 0, new QTableWidgetItem(p.getCodigo()));
        ui.tablaProductos->setItem(i, 1, new QTableWidgetItem(p.getNombre()));
        ui.tablaProductos->setItem(i, 2, new QTableWidgetItem(QString::number(p.getPrecio())));
        ui.tablaProductos->setItem(i, 3, new QTableWidgetItem(QString::number(p.getExistencias())));
        ui.tablaProductos->setItem(i, 4, new QTableWidgetItem(estado));
    }
}

// Al elegir una fila, sus datos pasan a los campos de la derecha
void ProductosForm::seleccionarProducto(int fila, int columna)
{
    if (fila == -1) {
        return;   // la tabla se quedó sin fila seleccionada
    }
    Producto p = inventario->getProducto(fila);
    ui.txtCodigo->setText(p.getCodigo());
    ui.txtNombre->setText(p.getNombre());
    ui.spnPrecio->setValue(p.getPrecio());
    ui.spnExistencias->setValue(p.getExistencias());
}

void ProductosForm::agregarProducto()
{
    Producto nuevo(ui.txtCodigo->text(), ui.txtNombre->text(), ui.spnPrecio->value(), ui.spnExistencias->value());
    QString error = inventario->agregar(nuevo);
    if (error != "") {
        QMessageBox::warning(this, "Error", error);
        return;
    }
    guardarArchivo();
}

void ProductosForm::modificarProducto()
{
    int fila = ui.tablaProductos->currentRow();
    if (fila == -1) {
        QMessageBox::warning(this, "Error", "Seleccione un producto de la tabla.");
        return;
    }
    QString error = inventario->modificar(fila, ui.txtNombre->text(), ui.spnPrecio->value(), ui.spnExistencias->value());
    if (error != "") {
        QMessageBox::warning(this, "Error", error);
        return;
    }
    guardarArchivo();
}

void ProductosForm::cambiarEstado()
{
    int fila = ui.tablaProductos->currentRow();
    if (fila == -1) {
        QMessageBox::warning(this, "Error", "Seleccione un producto de la tabla.");
        return;
    }
    inventario->cambiarEstado(fila);
    guardarArchivo();
}

void ProductosForm::guardarArchivo()
{
    if (ArchivoProductos::guardar(inventario) == false) {
        QMessageBox::critical(this, "Error", "No se pudo guardar productos.txt");
    }
    actualizar();
}
