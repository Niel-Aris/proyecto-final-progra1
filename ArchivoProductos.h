#pragma once
#include "Inventario.h"

// Guarda y lee los productos en productos.json y la actividad en movimientos.json
// productos.json: [ {"codigo": "P001", "nombre": "Arroz 1 kg", "precio": 1250, "existencias": 24, "activo": true}, ... ]
// movimientos.json: [ {"fecha": "17/11/2026 10:30", "codigo": "P001", "texto": "Precio: ₡1 250 → ₡1 400"}, ... ]
// Si todavía no hay productos.json pero sí el productos.txt de antes, se lee ese una vez
// y desde el siguiente guardado ya queda en JSON (así no se pierden los datos viejos).
// Los métodos son static: se usan sin crear un objeto, así: ArchivoProductos::guardar(inventario)
class ArchivoProductos
{
public:
    static bool guardar(Inventario* inventario);
    static bool cargar(Inventario* inventario);

private:
    static bool cargarTxt(Inventario* inventario);
};
