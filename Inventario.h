#pragma once
#include <QString>
#include <vector>
#include "Producto.h"

// Un cambio en el inventario: qué pasó, a qué producto y cuándo.
// Sirve para mostrar la actividad de cada producto.
class Movimiento
{
public:
    Movimiento(QString fecha, QString codigo, QString texto);

    QString getFecha() const;
    QString getCodigo() const;
    QString getTexto() const;

private:
    QString fecha;
    QString codigo;
    QString texto;
};

// Integrante 1: lista de productos y sus reglas.
// Los métodos que pueden fallar devuelven un mensaje de error, o "" si todo salió bien.
// Cada cambio queda anotado como Movimiento y los cambios hechos a mano se pueden deshacer.
class Inventario
{
public:
    QString agregar(Producto producto);
    QString modificar(int posicion, QString nombre, int precio, int existencias);
    QString ajustarExistencias(int posicion, int cantidad, QString motivo);   // cantidad puede ser negativa
    void cambiarEstado(int posicion);
    void rebajarExistencias(int posicion, int cantidad);   // la usa el carrito al cobrar

    int buscar(QString codigo) const;   // devuelve la posición, o -1 si no existe
    int getCantidad() const;
    Producto getProducto(int posicion) const;

    // Deshacer
    bool deshacer();
    bool puedeDeshacer() const;
    QString getUltimoCambio() const;   // texto corto del último cambio que se puede deshacer
    void limpiarHistorial();

    // Actividad
    void agregarMovimiento(Movimiento movimiento);
    int getCantidadMovimientos() const;
    Movimiento getMovimiento(int posicion) const;
    std::vector<Movimiento> getMovimientos(QString codigo) const;   // del más nuevo al más viejo

private:
    QString validar(QString nombre, int precio, int existencias) const;
    void guardarEstado(QString cambio);
    void registrar(QString codigo, QString texto);
    QString colones(int monto) const;

    std::vector<Producto> productos;
    std::vector<Movimiento> movimientos;

    // Para deshacer se guarda cómo estaba todo antes de cada cambio
    std::vector<std::vector<Producto>> estadosAnteriores;
    std::vector<int> movimientosAnteriores;
    std::vector<QString> cambios;
};
