#include "VentanaPrototipo.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    VentanaPrototipo ventana;
    ventana.show();

    return a.exec();
}
