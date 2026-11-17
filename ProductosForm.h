#pragma once
#include <QWidget>
#include <QTimer>
#include <QTableWidgetItem>
#include <QPixmap>
#include <QIcon>
#include <QPointer>
#include <QVariantAnimation>
#include <QResizeEvent>
#include "ui_ProductosForm.h"
#include "Inventario.h"

// Integrante 1: pantalla de productos.
// A la izquierda la lista con búsqueda y filtros. A la derecha la ficha del producto elegido:
// los cambios se guardan solos al terminar de escribir y se pueden deshacer.
class ProductosForm : public QWidget
{
    Q_OBJECT

public:
    ProductosForm(Inventario* inventario, QWidget* parent = nullptr);
    void actualizar();

private:
    // Lista
    void llenarTabla();
    void aplicarFiltro();
    void mostrarResumen();
    void seleccionarProducto(int fila, int columna);
    void editarEnTabla(QTableWidgetItem* item);
    int posicionDeFila(int fila) const;
    QString siguienteCodigo() const;

    // Ficha
    void nuevoProducto();
    void crearProducto();
    void guardarDatos();
    void sumarExistencias(int cantidad, QString motivo);
    void cambiarDisponible(bool disponible);
    void deshacer();
    void cargarCampos();
    void mostrarFicha();
    void mostrarActividad();

    // Avisos
    void cambioHecho();
    void mostrarBarra(QString texto);
    void mostrarError(QString error);
    void limpiarError();
    bool guardarArchivo();

    // Imágenes
    QPixmap imagenProducto(QString codigo, QString nombre, bool activo) const;
    QIcon miniatura(QString codigo, QString nombre, bool activo) const;
    void mostrarImagen(bool animar);
    void pintarImagen(double escala);
    void resizeEvent(QResizeEvent* evento) override;

    Ui::ProductosForm ui;
    Inventario* inventario;
    int posicionActual;     // posición del producto que se edita, o -1 si es uno nuevo
    bool llenando;          // true mientras se llena la tabla o la ficha, para no tomarlo como cambio
    QTimer timerBarra;      // esconde la barra de "Deshacer" después de unos segundos
    QPixmap imagenActual;   // la imagen que se está mostrando, en tamaño original
    QPointer<QVariantAnimation> animacionImagen;   // la animación de la imagen, si hay una corriendo
};
