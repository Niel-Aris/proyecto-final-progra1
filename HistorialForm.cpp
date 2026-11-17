#include "HistorialForm.h"
#include <QMessageBox>

HistorialForm::HistorialForm(Historial* historial, QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->historial = historial;

    formatos.push_back(&ticketTxt);
    formatos.push_back(&ticketCsv);
    for (int i = 0; i < formatos.size(); i++) {
        ui.cmbFormato->addItem(formatos[i]->getExtension());
    }

    connect(ui.tablaVentas, &QTableWidget::cellClicked, this, &HistorialForm::verTicket);
    connect(ui.btnGenerar, &QPushButton::clicked, this, &HistorialForm::generarTicket);

    actualizar();
}

void HistorialForm::actualizar()
{
    ui.tablaVentas->setRowCount(historial->getCantidad());
    for (int i = 0; i < historial->getCantidad(); i++) {
        Venta v = historial->getVenta(i);
        ui.tablaVentas->setItem(i, 0, new QTableWidgetItem(QString::number(v.getNumero())));
        ui.tablaVentas->setItem(i, 1, new QTableWidgetItem(v.getFecha()));
        ui.tablaVentas->setItem(i, 2, new QTableWidgetItem(QString::number(v.getTotal())));
    }
}

// Al hacer clic en una venta se muestra su ticket en texto
void HistorialForm::verTicket(int fila, int columna)
{
    Venta v = historial->getVenta(fila);
    ui.txtTicket->setPlainText(ticketTxt.generar(v));
}

void HistorialForm::generarTicket()
{
    int fila = ui.tablaVentas->currentRow();
    if (fila == -1) {
        QMessageBox::warning(this, "Error", "Seleccione una venta.");
        return;
    }
    Venta v = historial->getVenta(fila);

    // El mismo código sirve para cualquier formato (polimorfismo)
    Ticket* ticket = formatos[ui.cmbFormato->currentIndex()];
    ui.txtTicket->setPlainText(ticket->generar(v));

    if (ticket->guardar(v) == true) {
        QMessageBox::information(this, "Ticket", "Ticket guardado.");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo guardar el ticket.");
    }
}
