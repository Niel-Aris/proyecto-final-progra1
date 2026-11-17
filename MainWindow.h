#pragma once
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include "ui_MainWindow.h"
#include "Inventario.h"
#include "Historial.h"
#include "ProductosForm.h"
#include "VentasForm.h"
#include "HistorialForm.h"

// Ventana principal: el encabezado (logo, fecha, tema, ajustes y usuario)
// y las tres pantallas en pestañas
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void setUsuario(QString usuario);

private:
    void crearEncabezado();
    void cambiarPestana(int indice);
    void refrescarTodo();
    void cambiarTema();
    void abrirAjustes();
    void cerrarSesion();

    Ui::MainWindow ui;
    Inventario inventario;
    Historial historial;
    ProductosForm* productosForm;
    VentasForm* ventasForm;
    HistorialForm* historialForm;

    // Encabezado
    QLabel* lblMarca;
    QLabel* lblFecha;
    QPushButton* btnTema;
    QPushButton* btnAjustes;
    QPushButton* btnUsuario;
    QString usuario;
};
