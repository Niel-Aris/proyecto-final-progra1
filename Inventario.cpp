#include "Inventario.h"
#include <QDateTime>
#include <QLocale>
#include <cstdlib>

// Cuántos cambios se pueden deshacer
const int MAXIMO_DESHACER = 30;

Movimiento::Movimiento(QString fecha, QString codigo, QString texto)
{
    this->fecha = fecha;
    this->codigo = codigo;
    this->texto = texto;
}

QString Movimiento::getFecha() const
{
    return fecha;
}

QString Movimiento::getCodigo() const
{
    return codigo;
}

QString Movimiento::getTexto() const
{
    return texto;
}

QString Inventario::validar(QString nombre, int precio, int existencias) const
{
    if (nombre.trimmed() == "") {
        return "El nombre es obligatorio.";
    }
    if (nombre.contains(";")) {
        return "El nombre no puede tener el símbolo ;";
    }
    if (precio <= 0) {
        return "El precio debe ser mayor que cero.";
    }
    if (existencias < 0) {
        return "Las existencias no pueden ser negativas.";
    }
    return "";
}

QString Inventario::colones(int monto) const
{
    QLocale local(QLocale::Spanish, QLocale::CostaRica);
    return "₡" + local.toString(monto);
}

// Antes de cada cambio hecho a mano se guarda una copia, para poder deshacerlo
void Inventario::guardarEstado(QString cambio)
{
    estadosAnteriores.push_back(productos);
    movimientosAnteriores.push_back(movimientos.size());
    cambios.push_back(cambio);
    if (estadosAnteriores.size() > MAXIMO_DESHACER) {
        estadosAnteriores.erase(estadosAnteriores.begin());
        movimientosAnteriores.erase(movimientosAnteriores.begin());
        cambios.erase(cambios.begin());
    }
}

void Inventario::registrar(QString codigo, QString texto)
{
    QString fecha = QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm");
    movimientos.push_back(Movimiento(fecha, codigo, texto));
}

QString Inventario::agregar(Producto producto)
{
    if (producto.getCodigo().trimmed() == "") {
        return "El código es obligatorio.";
    }
    if (producto.getCodigo().contains(";")) {
        return "El código no puede tener el símbolo ;";
    }
    if (buscar(producto.getCodigo()) != -1) {
        return "Ya existe un producto con ese código.";
    }
    QString error = validar(producto.getNombre(), producto.getPrecio(), producto.getExistencias());
    if (error != "") {
        return error;
    }
    guardarEstado("Se creó " + producto.getNombre());
    productos.push_back(producto);
    registrar(producto.getCodigo(), "Creado con " + QString::number(producto.getExistencias())
                                        + " en existencia a " + colones(producto.getPrecio()));
    return "";
}

// Cambia nombre, precio y existencias. Solo anota lo que de verdad cambió.
QString Inventario::modificar(int posicion, QString nombre, int precio, int existencias)
{
    QString error = validar(nombre, precio, existencias);
    if (error != "") {
        return error;
    }
    Producto antes = productos[posicion];
    if (antes.getNombre() == nombre && antes.getPrecio() == precio && antes.getExistencias() == existencias) {
        return "";   // no cambió nada
    }

    QString cambio = "Se cambió " + antes.getNombre();
    if (antes.getPrecio() != precio) {
        cambio = "Precio de " + antes.getNombre() + ": " + colones(precio);
    } else if (antes.getExistencias() != existencias) {
        cambio = antes.getNombre() + ": " + QString::number(existencias) + " en existencia";
    } else if (antes.getNombre() != nombre) {
        cambio = "Nuevo nombre: " + nombre;
    }
    guardarEstado(cambio);

    productos[posicion].setNombre(nombre);
    productos[posicion].setPrecio(precio);
    productos[posicion].setExistencias(existencias);

    QString codigo = antes.getCodigo();
    if (antes.getNombre() != nombre) {
        registrar(codigo, "Nombre: " + antes.getNombre() + " → " + nombre);
    }
    if (antes.getPrecio() != precio) {
        registrar(codigo, "Precio: " + colones(antes.getPrecio()) + " → " + colones(precio));
    }
    if (antes.getExistencias() != existencias) {
        registrar(codigo, "Conteo: " + QString::number(antes.getExistencias()) + " → " + QString::number(existencias));
    }
    return "";
}

// Suma o resta existencias, por ejemplo cuando llega mercadería
QString Inventario::ajustarExistencias(int posicion, int cantidad, QString motivo)
{
    if (cantidad == 0) {
        return "";
    }
    Producto antes = productos[posicion];
    int nuevas = antes.getExistencias() + cantidad;
    if (nuevas < 0) {
        return "No quedan existencias para restar.";
    }
    QString signo = "+";
    if (cantidad < 0) {
        signo = "−";
    }
    QString cantidadTexto = signo + QString::number(abs(cantidad));
    guardarEstado(motivo + " " + cantidadTexto + " de " + antes.getNombre());

    productos[posicion].setExistencias(nuevas);
    registrar(antes.getCodigo(), motivo + " " + cantidadTexto + "  ("
                                     + QString::number(antes.getExistencias()) + " → " + QString::number(nuevas) + ")");
    return "";
}

void Inventario::cambiarEstado(int posicion)
{
    Producto antes = productos[posicion];
    if (antes.getActivo() == true) {
        guardarEstado("Se desactivó " + antes.getNombre());
        productos[posicion].setActivo(false);
        registrar(antes.getCodigo(), "Desactivado: ya no aparece en Vender");
    } else {
        guardarEstado("Se activó " + antes.getNombre());
        productos[posicion].setActivo(true);
        registrar(antes.getCodigo(), "Activado de nuevo");
    }
}

// La usa el carrito al cobrar. Una venta no se deshace desde productos,
// por eso se borra lo que había para deshacer (si no, deshacer traería de vuelta lo vendido).
void Inventario::rebajarExistencias(int posicion, int cantidad)
{
    int antes = productos[posicion].getExistencias();
    int nuevas = antes - cantidad;
    productos[posicion].setExistencias(nuevas);
    registrar(productos[posicion].getCodigo(), "Vendido −" + QString::number(cantidad) + "  ("
                                                   + QString::number(antes) + " → " + QString::number(nuevas) + ")");
    estadosAnteriores.clear();
    movimientosAnteriores.clear();
    cambios.clear();
}

bool Inventario::deshacer()
{
    if (estadosAnteriores.size() == 0) {
        return false;
    }
    productos = estadosAnteriores.back();
    movimientos.resize(movimientosAnteriores.back(), Movimiento("", "", ""));
    estadosAnteriores.pop_back();
    movimientosAnteriores.pop_back();
    cambios.pop_back();
    return true;
}

bool Inventario::puedeDeshacer() const
{
    return estadosAnteriores.size() > 0;
}

QString Inventario::getUltimoCambio() const
{
    if (cambios.size() == 0) {
        return "";
    }
    return cambios.back();
}

// Se usa al cargar el archivo, para que cargar los productos no cuente como cambios
void Inventario::limpiarHistorial()
{
    movimientos.clear();
    estadosAnteriores.clear();
    movimientosAnteriores.clear();
    cambios.clear();
}

void Inventario::agregarMovimiento(Movimiento movimiento)
{
    movimientos.push_back(movimiento);
}

int Inventario::getCantidadMovimientos() const
{
    return movimientos.size();
}

Movimiento Inventario::getMovimiento(int posicion) const
{
    return movimientos[posicion];
}

std::vector<Movimiento> Inventario::getMovimientos(QString codigo) const
{
    std::vector<Movimiento> lista;
    for (int i = movimientos.size() - 1; i >= 0; i--) {
        if (movimientos[i].getCodigo() == codigo) {
            lista.push_back(movimientos[i]);
        }
    }
    return lista;
}

int Inventario::buscar(QString codigo) const
{
    for (int i = 0; i < productos.size(); i++) {
        if (productos[i].getCodigo() == codigo) {
            return i;
        }
    }
    return -1;
}

int Inventario::getCantidad() const
{
    return productos.size();
}

Producto Inventario::getProducto(int posicion) const
{
    return productos[posicion];
}
