#pragma once
#include <QApplication>
#include <QMainWindow>
#include <QColor>
#include <functional>

// Capa visual de La Pulpe: colores, estilo y animaciones.
// No cambia cómo funcionan las pantallas, solo cómo se ven.
// Se usa desde main.cpp, sin tocar el código de los formularios.
class Estilo
{
public:
    // Pone el estilo Fusion, los colores del tema y la hoja estilo.qss
    static void cargar(QApplication* app);

    // Vuelve a poner los colores según el ajuste de tema (claro, oscuro o automático)
    static void aplicarTema();

    // Color del tema actual, para lo que se dibuja con código (por ejemplo "verde" o "fondo")
    static QColor color(QString nombre);
    static bool esOscuro();

    // Si alguien pone algo aquí, se llama cada vez que cambia el tema
    static std::function<void()> alCambiarTema;

    // Arregla la ventana ya creada: encabezado, tablas, espacios y animación de pestañas
    static void aplicar(QMainWindow* ventana);

    // La ventana aparece suave al abrir el programa
    static void animarEntrada(QMainWindow* ventana);

    // La pantalla aparece y sube un poco, como en las apps de Apple
    static void animarPagina(QWidget* pagina);

    // Qt no puede dibujar un efecto dentro de otro. Antes de animar algo dentro de una
    // pantalla se llama esto: si la pantalla todavía está apareciendo, se termina de una vez.
    static void terminarEfectosArriba(QWidget* widget);

private:
    static void aparecer(QWidget* widget, int duracion, int desplazamiento);
};
