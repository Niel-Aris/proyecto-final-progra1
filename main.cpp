#include "MainWindow.h"
#include "Estilo.h"
#include "Ajustes.h"
#include "VentanaLogin.h"
#include <QtWidgets/QApplication>
#include <QIcon>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    // Con estos nombres QSettings sabe dónde guardar los ajustes
    a.setOrganizationName("LaPulpe");
    a.setApplicationName("La Pulpe");
    a.setWindowIcon(QIcon(":/imagenes/logo.png"));
    Estilo::cargar(&a);

    // Inicio de sesión de prueba (se puede apagar en Ajustes)
    if (Ajustes::pedirPin() == true) {
        VentanaLogin login;
        if (login.exec() != QDialog::Accepted) {
            return 0;
        }
    }

    MainWindow w;
    Estilo::aplicar(&w);
    w.show();
    Estilo::animarEntrada(&w);

    return a.exec();
}
