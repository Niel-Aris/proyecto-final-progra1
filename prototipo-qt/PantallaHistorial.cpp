#include "PantallaHistorial.h"
#include <QColor>
#include <QComboBox>
#include <QFont>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PantallaHistorial::PantallaHistorial(QWidget* parent)
    : QWidget(parent)
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    paginas = new QStackedWidget();
    layout->addWidget(paginas);

    QString ticketTxt = "PULPERIA LA PULPE\n"
                        "Venta #1   09/11/2026 10:30\n"
                        "------------------------------\n"
                        "2 x Arroz 1 kg   2500\n"
                        "1 x Leche 1 L   975\n"
                        "------------------------------\n"
                        "Total:  3475\n"
                        "Pago:   5000\n"
                        "Cambio: 1525\n";

    QString ticketCsv = "producto,cantidad,precio,subtotal\n"
                        "Arroz 1 kg,2,1250,2500\n"
                        "Leche 1 L,1,975,975\n"
                        "total,,,3475\n"
                        "pago,,,5000\n"
                        "cambio,,,1525\n";

    paginas->addWidget(crearPagina(0, "txt", "", "Clic en la venta para ver su ticket."));
    paginas->addWidget(crearPagina(1, "txt", ticketTxt, "Clic en Generar ticket para guardarlo en otro formato."));
    paginas->addWidget(crearPagina(2, "csv", ticketCsv, "El mismo botón sirve para los dos formatos."));
}

QWidget* PantallaHistorial::crearPagina(int numero, QString formato, QString ticket, QString nota)
{
    QTableWidget* tabla = new QTableWidget(1, 3);
    tabla->setHorizontalHeaderLabels({"Venta", "Fecha", "Total"});
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionMode(QAbstractItemView::NoSelection);
    tabla->horizontalHeader()->setStretchLastSection(true);

    QString datos[3] = {"1", "09/11/2026 10:30", "3475"};
    for (int j = 0; j < 3; j++) {
        QTableWidgetItem* celda = new QTableWidgetItem(datos[j]);
        if (numero != 0) {
            celda->setBackground(QColor(204, 232, 255));
        }
        tabla->setItem(0, j, celda);
    }

    QComboBox* cmbFormato = new QComboBox();
    cmbFormato->addItem(formato);

    QPushButton* btnGenerar = new QPushButton("Generar ticket");

    QHBoxLayout* filaFormato = new QHBoxLayout();
    filaFormato->addWidget(new QLabel("Formato"));
    filaFormato->addWidget(cmbFormato);
    filaFormato->addWidget(btnGenerar);

    QPlainTextEdit* txtTicket = new QPlainTextEdit(ticket);
    txtTicket->setReadOnly(true);
    txtTicket->setFont(QFont("Courier New"));

    QVBoxLayout* derecha = new QVBoxLayout();
    derecha->addLayout(filaFormato);
    derecha->addWidget(txtTicket);

    // Cada página solo conecta los clics que se quieren enseñar
    if (numero == 0) {
        connect(tabla, &QTableWidget::cellClicked, this, &PantallaHistorial::verTicket);
    }
    if (numero == 1) {
        connect(btnGenerar, &QPushButton::clicked, this, &PantallaHistorial::generarTicket);
    }

    QHBoxLayout* contenido = new QHBoxLayout();
    contenido->addWidget(tabla, 1);
    contenido->addLayout(derecha, 1);

    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);
    layout->addLayout(contenido);
    layout->addWidget(new QLabel(nota));
    return pagina;
}

void PantallaHistorial::verTicket(int fila, int columna)
{
    paginas->setCurrentIndex(1);
}

void PantallaHistorial::generarTicket()
{
    paginas->setCurrentIndex(2);
    QMessageBox::information(this, "Ticket", "Ticket guardado.");
    paginas->setCurrentIndex(1);
}
