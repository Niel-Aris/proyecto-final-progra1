#pragma once
#include <QString>

// Ajustes generales de la aplicación. Se guardan solos en la computadora con QSettings,
// así se mantienen aunque se cierre el programa.
// Los métodos son static: se usan así, Ajustes::tema()
class Ajustes
{
public:
    // "auto" (igual que el sistema), "claro" u "oscuro"
    static QString tema();
    static void setTema(QString tema);

    static bool animaciones();
    static void setAnimaciones(bool activas);

    static QString nombreTienda();
    static void setNombreTienda(QString nombre);

    // Desde cuántas unidades para abajo un producto sale como "por agotarse"
    static int pocasExistencias();
    static void setPocasExistencias(int cantidad);

    // Inicio de sesión de prueba
    static bool pedirPin();
    static void setPedirPin(bool pedir);
    static QString ultimoUsuario();
    static void setUltimoUsuario(QString usuario);
};
