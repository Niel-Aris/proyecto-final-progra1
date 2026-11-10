#include "PantallaProductos.h"
#include <QColor>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PantallaProductos::PantallaProductos(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    paginas = new QStackedWidget();
    layout->addWidget(paginas);

    paginas->addWidget(crearPagina(0, -1, "Activo", "", "", "0", "0",
                                   "Clic en la fila de Café para elegirlo. Clic en Agregar para ver qué pasa con un dato malo."));
    paginas->addWidget(crearPagina(1, -1, "Activo", "P010", "Galletas", "0", "20",
                                   "Si el precio es 0 sale un mensaje y el producto no se guarda."));
    paginas->addWidget(crearPagina(2, 3, "Activo", "P004", "Café 250 g", "2350", "10",
                                   "Los datos pasan a los campos. Desde aquí se modifica o se desactiva."));
    paginas->addWidget(crearPagina(3, 3, "Inactivo", "P004", "Café 250 g", "2350", "10",
                                   "El producto no se borra, queda Inactivo y ya no sale en Vender."));
}

QWidget* PantallaProductos::crearPagina(int numero, int filaElegida, QString estadoCafe, QString codigo,
                                        QString nombre, QString precio, QString existencias, QString nota)
{
    QString datos[6][5] = {
        {"P001", "Arroz 1 kg", "1250", "24", "Activo"},
        {"P002", "Frijoles 900 g", "1475", "18", "Activo"},
        {"P003", "Leche 1 L", "975", "30", "Activo"},
        {"P004", "Café 250 g", "2350", "10", estadoCafe},
        {"P005", "Huevos (15)", "2900", "2", "Activo"},
        {"P006", "Refresco 2.5 L", "1850", "15", "Inactivo"}
    };

    QTableWidget* tabla = new QTableWidget(6, 5);
    tabla->setHorizontalHeaderLabels({"Código", "Nombre", "Precio", "Existencias", "Estado"});
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionMode(QAbstractItemView::NoSelection);
    tabla->horizontalHeader()->setStretchLastSection(true);

    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 5; j++) {
            QTableWidgetItem* celda = new QTableWidgetItem(datos[i][j]);
            if (i == filaElegida) {
                celda->setBackground(QColor(204, 232, 255));
            }
            tabla->setItem(i, j, celda);
        }
    }

    QLineEdit* txtCodigo = new QLineEdit(codigo);
    QLineEdit* txtNombre = new QLineEdit(nombre);
    QLineEdit* txtPrecio = new QLineEdit(precio);
    QLineEdit* txtExistencias = new QLineEdit(existencias);
    txtCodigo->setReadOnly(true);
    txtNombre->setReadOnly(true);
    txtPrecio->setReadOnly(true);
    txtExistencias->setReadOnly(true);

    QFormLayout* campos = new QFormLayout();
    campos->addRow("Código", txtCodigo);
    campos->addRow("Nombre", txtNombre);
    campos->addRow("Precio ₡", txtPrecio);
    campos->addRow("Existencias", txtExistencias);

    QPushButton* btnAgregar = new QPushButton("Agregar");
    QPushButton* btnModificar = new QPushButton("Modificar");
    QPushButton* btnDesactivar = new QPushButton("Activar / Desactivar");

    QVBoxLayout* ficha = new QVBoxLayout();
    ficha->addLayout(campos);
    ficha->addWidget(btnAgregar);
    ficha->addWidget(btnModificar);
    ficha->addWidget(btnDesactivar);

    // Cada página solo conecta los clics que se quieren enseñar
    if (numero == 0) {
        connect(btnAgregar, &QPushButton::clicked, this, &PantallaProductos::mostrarError);
        connect(tabla, &QTableWidget::cellClicked, this, &PantallaProductos::elegirFila);
    }
    if (numero == 2) {
        connect(btnDesactivar, &QPushButton::clicked, this, &PantallaProductos::desactivar);
    }
    if (numero == 2 || numero == 3) {
        QPushButton* btnVolver = new QPushButton("Volver a la lista");
        ficha->addWidget(btnVolver);
        connect(btnVolver, &QPushButton::clicked, this, &PantallaProductos::verLista);
    }
    ficha->addStretch();

    QHBoxLayout* contenido = new QHBoxLayout();
    contenido->addWidget(tabla, 1);
    contenido->addLayout(ficha);

    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);
    layout->addLayout(contenido);
    layout->addWidget(new QLabel(nota));
    return pagina;
}

void PantallaProductos::mostrarError()
{
    paginas->setCurrentIndex(1);
    QMessageBox::warning(this, "Error", "El precio debe ser mayor que cero.");
    paginas->setCurrentIndex(0);
}

void PantallaProductos::elegirFila(int fila, int columna)
{
    if (fila == 3) {
        paginas->setCurrentIndex(2);
    }
}

void PantallaProductos::desactivar()
{
    paginas->setCurrentIndex(3);
}

void PantallaProductos::verLista()
{
    paginas->setCurrentIndex(0);
}
