#pragma once
#include <QWidget>
#include <vector>
#include "ui_HistorialForm.h"
#include "Historial.h"
#include "TicketTxt.h"
#include "TicketCsv.h"

// Integrante 3: pantalla del historial de ventas
class HistorialForm : public QWidget
{
    Q_OBJECT

public:
    HistorialForm(Historial* historial, QWidget* parent = nullptr);
    void actualizar();

private:
    void verTicket(int fila, int columna);
    void generarTicket();

    Ui::HistorialForm ui;
    Historial* historial;

    // Formatos de ticket. Para agregar uno nuevo se agrega a esta lista.
    TicketTxt ticketTxt;
    TicketCsv ticketCsv;
    std::vector<Ticket*> formatos;
};
