#pragma once
#include "Inventario.h"

// Guarda y lee los productos en productos.txt
// Cada línea: codigo;nombre;precio;existencias;activo
// Los métodos son static: se usan sin crear un objeto, así: ArchivoProductos::guardar(inventario)
class ArchivoProductos
{
public:
    static bool guardar(Inventario* inventario);
    static bool cargar(Inventario* inventario);
};
