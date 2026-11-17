#pragma once
#include <QString>
#include "Venta.h"

// Clase abstracta: cada formato de ticket es una clase hija.
// Para agregar un formato nuevo se crea otra clase hija, sin cambiar esta.
class Ticket
{
public:
    virtual QString generar(Venta venta) = 0;
    virtual QString getExtension() = 0;

    bool guardar(Venta venta);
};
