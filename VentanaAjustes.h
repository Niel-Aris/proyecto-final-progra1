#pragma once
#include <QDialog>
#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include "Interruptor.h"

// Ventana de ajustes generales: tema, animaciones, datos de la pulpería y sesión.
// Los cambios se aplican apenas se tocan; "Listo" solo cierra la ventana.
class VentanaAjustes : public QDialog
{
    Q_OBJECT

public:
    VentanaAjustes(QWidget* parent = nullptr);

private:
    QFrame* crearTarjeta(QString titulo, QVBoxLayout* columna);
    void agregarFila(QFrame* tarjeta, QString texto, QString detalle, QWidget* control);
    void elegirTema(QString tema);

    QPushButton* btnAuto;
    QPushButton* btnClaro;
    QPushButton* btnOscuro;
};
