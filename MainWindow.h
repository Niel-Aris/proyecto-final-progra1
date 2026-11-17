#pragma once
#include <QMainWindow>
#include "ui_MainWindow.h"
#include "Inventario.h"
#include "Historial.h"
#include "ProductosForm.h"
#include "VentasForm.h"
#include "HistorialForm.h"

// Ventana principal: aquí se integran las tres pantallas en pestañas
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private:
    void cambiarPestana(int indice);

    Ui::MainWindow ui;
    Inventario inventario;
    Historial historial;
    ProductosForm* productosForm;
    VentasForm* ventasForm;
    HistorialForm* historialForm;
};
