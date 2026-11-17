#pragma once
#include <QApplication>
#include <QMainWindow>

// Capa visual de La Pulpe: colores, estilo y animaciones.
// No cambia cómo funcionan las pantallas, solo cómo se ven.
// Se usa desde main.cpp, sin tocar el código de los formularios.
class Estilo
{
public:
    // Pone el estilo Fusion, los colores claros y la hoja estilo.qss
    static void cargar(QApplication* app);

    // Arregla la ventana ya creada: encabezado, tablas, espacios y animación de pestañas
    static void aplicar(QMainWindow* ventana);

    // La ventana aparece suave al abrir el programa
    static void animarEntrada(QMainWindow* ventana);

    // La pantalla aparece y sube un poco, como en las apps de Apple
    static void animarPagina(QWidget* pagina);

private:
    static void aparecer(QWidget* widget, int duracion, int desplazamiento);
};
