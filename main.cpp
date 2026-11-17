#include "MainWindow.h"
#include "Estilo.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    Estilo::cargar(&a);

    MainWindow w;
    Estilo::aplicar(&w);
    w.show();
    Estilo::animarEntrada(&w);

    return a.exec();
}
