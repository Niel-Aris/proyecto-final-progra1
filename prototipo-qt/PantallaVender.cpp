#include "PantallaVender.h"
#include <QComboBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PantallaVender::PantallaVender(QTabWidget* pestanas, QWidget* parent)
    : QWidget(parent)
{
    this->pestanas = pestanas;

    QVBoxLayout* layout = new QVBoxLayout(this);
    paginas = new QStackedWidget();
    layout->addWidget(paginas);

    paginas->addWidget(crearPagina(0, "Arroz 1 kg  (₡1250, quedan 24)", "2", false, "0", "0",
                                   "Clic en Agregar al carrito para seguir con la venta."));
    paginas->addWidget(crearPagina(1, "Leche 1 L  (₡975, quedan 30)", "1", true, "3475", "5000",
                                   "Clic en Cobrar para terminar la venta."));
    paginas->addWidget(crearPagina(2, "Leche 1 L  (₡975, quedan 30)", "1", true, "3475", "1000",
                                   "No se guarda nada. Aceptar vuelve al carrito para corregir el pago."));
    paginas->addWidget(crearPagina(3, "Huevos (15)  (₡2900, quedan 2)", "3", false, "0", "0",
                                   "Solo quedan 2 y se piden 3. Aceptar vuelve a Vender."));
}

QWidget* PantallaVender::crearPagina(int numero, QString producto, QString cantidad, bool conLineas,
                                     QString total, QString pago, QString nota)
{
    QComboBox* cmbProductos = new QComboBox();
    cmbProductos->addItem(producto);

    QLineEdit* txtCantidad = new QLineEdit(cantidad);
    txtCantidad->setReadOnly(true);
    txtCantidad->setMaximumWidth(70);

    QPushButton* btnAgregar = new QPushButton("Agregar al carrito");

    QHBoxLayout* filaArriba = new QHBoxLayout();
    filaArriba->addWidget(new QLabel("Producto"));
    filaArriba->addWidget(cmbProductos, 1);
    filaArriba->addWidget(new QLabel("Cantidad"));
    filaArriba->addWidget(txtCantidad);
    filaArriba->addWidget(btnAgregar);

    QTableWidget* tabla = new QTableWidget(0, 4);
    tabla->setHorizontalHeaderLabels({"Producto", "Cantidad", "Precio", "Subtotal"});
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionMode(QAbstractItemView::NoSelection);
    tabla->horizontalHeader()->setStretchLastSection(true);

    if (conLineas == true) {
        QString datos[2][4] = {
            {"Arroz 1 kg", "2", "1250", "2500"},
            {"Leche 1 L", "1", "975", "975"}
        };
        tabla->setRowCount(2);
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 4; j++) {
                tabla->setItem(i, j, new QTableWidgetItem(datos[i][j]));
            }
        }
    }

    QLabel* lblTotal = new QLabel("Total: ₡" + total);
    QFont letra = lblTotal->font();
    letra.setBold(true);
    letra.setPointSize(letra.pointSize() + 3);
    lblTotal->setFont(letra);

    QLineEdit* txtPago = new QLineEdit(pago);
    txtPago->setReadOnly(true);
    txtPago->setMaximumWidth(110);

    QPushButton* btnCobrar = new QPushButton("Cobrar");
    QPushButton* btnCancelar = new QPushButton("Cancelar venta");

    QHBoxLayout* filaAbajo = new QHBoxLayout();
    filaAbajo->addWidget(lblTotal);
    filaAbajo->addStretch();
    filaAbajo->addWidget(new QLabel("Paga con ₡"));
    filaAbajo->addWidget(txtPago);
    filaAbajo->addWidget(btnCobrar);
    filaAbajo->addWidget(btnCancelar);

    QHBoxLayout* filaNota = new QHBoxLayout();
    filaNota->addWidget(new QLabel(nota));
    filaNota->addStretch();

    // Cada página solo conecta los clics que se quieren enseñar
    if (numero == 0) {
        connect(btnAgregar, &QPushButton::clicked, this, &PantallaVender::verCarrito);
        QPushButton* btnCaso = new QPushButton("Ver caso: sin existencias");
        filaNota->addWidget(btnCaso);
        connect(btnCaso, &QPushButton::clicked, this, &PantallaVender::verSinExistencias);
    }
    if (numero == 1) {
        connect(btnCobrar, &QPushButton::clicked, this, &PantallaVender::cobrar);
        connect(btnCancelar, &QPushButton::clicked, this, &PantallaVender::cancelar);
        QPushButton* btnCaso = new QPushButton("Ver caso: pago insuficiente");
        filaNota->addWidget(btnCaso);
        connect(btnCaso, &QPushButton::clicked, this, &PantallaVender::verPagoInsuficiente);
    }

    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);
    layout->addLayout(filaArriba);
    layout->addWidget(tabla);
    layout->addLayout(filaAbajo);
    layout->addLayout(filaNota);
    return pagina;
}

void PantallaVender::verCarrito()
{
    paginas->setCurrentIndex(1);
}

void PantallaVender::cobrar()
{
    QMessageBox::information(this, "Venta registrada", "Total: ₡3475\nCambio: ₡1525");
    paginas->setCurrentIndex(0);
    pestanas->setCurrentIndex(2);   // pestaña Historial
}

void PantallaVender::cancelar()
{
    paginas->setCurrentIndex(0);
}

void PantallaVender::verSinExistencias()
{
    paginas->setCurrentIndex(3);
    QMessageBox::warning(this, "No se puede agregar", "No hay existencias suficientes de Huevos (15).");
    paginas->setCurrentIndex(0);
}

void PantallaVender::verPagoInsuficiente()
{
    paginas->setCurrentIndex(2);
    QMessageBox::warning(this, "Venta rechazada", "Pago insuficiente.");
    paginas->setCurrentIndex(1);
}
