#include "Ajustes.h"
#include <QSettings>

QString Ajustes::tema()
{
    QSettings ajustes;
    return ajustes.value("apariencia/tema", "auto").toString();
}

void Ajustes::setTema(QString tema)
{
    QSettings ajustes;
    ajustes.setValue("apariencia/tema", tema);
}

bool Ajustes::animaciones()
{
    QSettings ajustes;
    return ajustes.value("apariencia/animaciones", true).toBool();
}

void Ajustes::setAnimaciones(bool activas)
{
    QSettings ajustes;
    ajustes.setValue("apariencia/animaciones", activas);
}

QString Ajustes::nombreTienda()
{
    QSettings ajustes;
    QString nombre = ajustes.value("pulperia/nombre", "La Pulpe").toString().trimmed();
    if (nombre == "") {
        nombre = "La Pulpe";
    }
    return nombre;
}

void Ajustes::setNombreTienda(QString nombre)
{
    QSettings ajustes;
    ajustes.setValue("pulperia/nombre", nombre.trimmed());
}

int Ajustes::pocasExistencias()
{
    QSettings ajustes;
    return ajustes.value("pulperia/pocasExistencias", 5).toInt();
}

void Ajustes::setPocasExistencias(int cantidad)
{
    QSettings ajustes;
    ajustes.setValue("pulperia/pocasExistencias", cantidad);
}

bool Ajustes::pedirPin()
{
    QSettings ajustes;
    return ajustes.value("sesion/pedirPin", true).toBool();
}

void Ajustes::setPedirPin(bool pedir)
{
    QSettings ajustes;
    ajustes.setValue("sesion/pedirPin", pedir);
}

QString Ajustes::ultimoUsuario()
{
    QSettings ajustes;
    return ajustes.value("sesion/ultimoUsuario", "Administrador").toString();
}

void Ajustes::setUltimoUsuario(QString usuario)
{
    QSettings ajustes;
    ajustes.setValue("sesion/ultimoUsuario", usuario);
}
