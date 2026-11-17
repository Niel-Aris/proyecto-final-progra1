#include "VentanaPrototipo.h"
#include "PantallaProductos.h"
#include "PantallaVender.h"
#include "PantallaHistorial.h"
#include <QLabel>
#include <QVBoxLayout>

VentanaPrototipo::VentanaPrototipo(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle("La Pulpe");
    resize(900, 550);

    pestanas = new QTabWidget();
    pestanas->addTab(new PantallaProductos(), "Productos");
    pestanas->addTab(new PantallaVender(pestanas), "Vender");
    pestanas->addTab(new PantallaHistorial(), "Historial");
    pestanas->addTab(crearEstilos(), "Estilos");

    setCentralWidget(pestanas);
}

// Pestaña con los colores, la letra y los controles que se van a usar
QWidget* VentanaPrototipo::crearEstilos()
{
    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);

    QLabel* texto = new QLabel(
        "Colores: los normales de Windows y celeste para la fila elegida.\n\n"
        "Letra: la normal de Windows en las pantallas y Courier New en el ticket.\n\n"
        "Controles:\n"
        "- Pestañas: QTabWidget\n"
        "- Tablas: QTableWidget\n"
        "- Campos de texto: QLineEdit\n"
        "- Lista de productos y formato: QComboBox\n"
        "- Botones: QPushButton\n"
        "- Mensajes: QMessageBox\n\n"
        "Navegación: las pestañas cambian de pantalla. Los errores salen en un mensaje\n"
        "y al darle Aceptar se vuelve a la misma pantalla.");
    layout->addWidget(texto);
    layout->addStretch();

    return pagina;
}
