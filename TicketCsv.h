#pragma once
#include "Ticket.h"

class TicketCsv : public Ticket
{
public:
    QString generar(Venta venta) override;
    QString getExtension() override;
};
