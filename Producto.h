#pragma once
#include <QString>

class Producto
{
public:
    Producto(QString codigo, QString nombre, int precio, int existencias);

    QString getCodigo() const;
    QString getNombre() const;
    int getPrecio() const;
    int getExistencias() const;
    bool getActivo() const;

    void setNombre(QString nombre);
    void setPrecio(int precio);
    void setExistencias(int existencias);
    void setActivo(bool activo);

private:
    QString codigo;
    QString nombre;
    int precio;
    int existencias;
    bool activo;
};
