#include "MainWindow.h"
#include "ArchivoProductos.h"
#include "ArchivoVentas.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Leer los datos guardados
    if (ArchivoProductos::cargar(&inventario) == false || ArchivoVentas::cargar(&historial) == false) {
        QMessageBox::warning(this, "Error", "No se pudieron leer los archivos de datos.");
    }

    // Crear las tres pantallas y ponerlas en pestañas
    productosForm = new ProductosForm(&inventario);
    ventasForm = new VentasForm(&inventario, &historial);
    historialForm = new HistorialForm(&historial);

    ui.tabWidget->addTab(productosForm, "Productos");
    ui.tabWidget->addTab(ventasForm, "Vender");
    ui.tabWidget->addTab(historialForm, "Historial");

    connect(ui.tabWidget, &QTabWidget::currentChanged, this, &MainWindow::cambiarPestana);
}

// Cada vez que se cambia de pestaña, las pantallas vuelven a leer los datos
void MainWindow::cambiarPestana(int indice)
{
    productosForm->actualizar();
    historialForm->actualizar();
    // Falta ventasForm->actualizar(): se agrega cuando se una el arreglo de VentasForm de Nahomy
}
