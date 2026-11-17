#pragma once
#include <QWidget>
#include <QTimer>
#include <QTableWidgetItem>
#include <QPixmap>
#include <QPointer>
#include <QVariantAnimation>
#include <QResizeEvent>
#include "ui_ProductosForm.h"
#include "Inventario.h"

// Integrante 1: pantalla de productos
class ProductosForm : public QWidget
{
    Q_OBJECT

public:
    ProductosForm(Inventario* inventario, QWidget* parent = nullptr);
    void actualizar();

private:
    void seleccionarProducto(int fila, int columna);
    void nuevoProducto();
    void guardarProducto();
    void cambiarEstado();
    void recibirMercaderia();
    void editarEnTabla(QTableWidgetItem* item);
    void aplicarFiltro();

    void llenarTabla();
    void mostrarFicha();
    void cargarCampos();
    void mostrarResumen();
    void mostrarMensaje(QString texto, bool esError);
    void enfocarCampo(QString error);
    QString siguienteCodigo() const;
    int posicionDeFila(int fila) const;
    bool guardarArchivo();

    // Imagen del producto en la ficha
    QPixmap imagenProducto(QString codigo, QString nombre, bool activo) const;
    void mostrarImagen(bool animar);
    void pintarImagen(double escala);
    void resizeEvent(QResizeEvent* evento) override;

    Ui::ProductosForm ui;
    Inventario* inventario;
    int posicionActual;     // posición del producto que se edita, o -1 si es uno nuevo
    bool llenando;          // true mientras se llena la tabla, para no tomarlo como edición
    QTimer timerMensaje;    // borra el mensaje después de unos segundos
    QPixmap imagenActual;   // la imagen que se está mostrando, en tamaño original
    QPointer<QVariantAnimation> animacionImagen;   // la animación de la imagen, si hay una corriendo
};
