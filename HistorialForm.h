#pragma once
#include <QWidget>
#include <QTimer>
#include <QListWidgetItem>
#include <vector>
#include "ui_HistorialForm.h"
#include "Historial.h"
#include "TicketTxt.h"
#include "TicketCsv.h"
#include "VistaTicket.h"

// Integrante 3: pantalla del historial de ventas.
// Arriba un resumen (hoy, venta promedio y lo más vendido), a la izquierda las ventas
// agrupadas por día y a la derecha el ticket de la venta elegida, dibujado como un recibo.
class HistorialForm : public QWidget
{
    Q_OBJECT

public:
    HistorialForm(Historial* historial, QWidget* parent = nullptr);
    void actualizar();

private:
    void mostrarResumen();
    void llenarLista();
    QWidget* crearFila(Venta venta);
    bool pasaFiltro(Venta venta) const;
    void elegirVenta(QListWidgetItem* actual);
    void guardarTicket(int formato);
    void copiarTicket();
    void mostrarAviso(QString texto, bool esError);

    QString colones(int monto) const;
    QString diaDe(QString fecha) const;   // "Hoy", "Ayer" o la fecha
    QString horaDe(QString fecha) const;

    Ui::HistorialForm ui;
    Historial* historial;
    VistaTicket* vistaTicket;
    int ventaElegida;       // posición en el historial de la venta que se ve, o -1
    QTimer timerAviso;

    // Formatos de ticket. Para agregar uno nuevo se agrega a esta lista
    // y aparece solo su botón de "Guardar" (polimorfismo).
    TicketTxt ticketTxt;
    TicketCsv ticketCsv;
    std::vector<Ticket*> formatos;
};
