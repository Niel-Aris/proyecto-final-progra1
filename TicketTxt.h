#pragma once
#include "Ticket.h"

class TicketTxt : public Ticket
{
public:
    QString generar(Venta venta) override;
    QString getExtension() override;
};
