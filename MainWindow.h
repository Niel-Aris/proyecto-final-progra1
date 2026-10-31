#pragma once
#include <QMainWindow>
#include "ui_MainWindow.h"

// Ventana principal: aquí se integran las tres pantallas en pestañas
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);

private:
    Ui::MainWindow ui;
};
