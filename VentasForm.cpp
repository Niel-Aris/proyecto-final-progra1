#include "VentasForm.h"
#include "ArchivoProductos.h"
#include "ArchivoVentas.h"
#include "TicketTxt.h"
#include "Imagenes.h"
#include "Estilo.h"
#include "Ajustes.h"
#include <QShortcut>
#include <QKeySequence>
#include <QLocale>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QStyle>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <vector>

const int LADO_FOTO = 92;

// Dibuja el círculo verde con el cheque de "venta lista". escala va de 0 a 1.
static QPixmap dibujarCheque(double escala)
{
    QPixmap imagen(192, 192);
    imagen.fill(Qt::transparent);
    imagen.setDevicePixelRatio(2.0);
    QPainter pintor(&imagen);
    pintor.setRenderHint(QPainter::Antialiasing);
    double radio = 40 * escala;
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QColor("#34c759"));
    pintor.drawEllipse(QPointF(48, 48), radio, radio);
    if (escala > 0.5) {
        QPen pluma(QColor("#ffffff"), 7 * escala, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        pintor.setPen(pluma);
        QPainterPath cheque;
        cheque.moveTo(48 - 17 * escala, 48 + 1 * escala);
        cheque.lineTo(48 - 5 * escala, 48 + 13 * escala);
        cheque.lineTo(48 + 18 * escala, 48 - 12 * escala);
        pintor.drawPath(cheque);
    }
    pintor.end();
    return imagen;
}

VentasForm::VentasForm(Inventario* inventario, Historial* historial, QWidget* parent)
    : QWidget(parent)
{
    this->inventario = inventario;
    this->historial = historial;
    ui.setupUi(this);
    totalMostrado = 0;

    // Catálogo: los productos como tarjetas con su imagen
    ui.lstProductos->setViewMode(QListView::IconMode);
    ui.lstProductos->setIconSize(QSize(LADO_FOTO, LADO_FOTO));
    ui.lstProductos->setGridSize(QSize(142, 176));
    ui.lstProductos->setMovement(QListView::Static);
    ui.lstProductos->setResizeMode(QListView::Adjust);
    ui.lstProductos->setWordWrap(true);
    ui.lstProductos->setUniformItemSizes(true);
    ui.lstProductos->setMouseTracking(true);
    ui.lstProductos->setCursor(Qt::PointingHandCursor);
    connect(ui.lstProductos, &QListWidget::itemClicked, this, &VentasForm::agregarDesdeCatalogo);
    connect(ui.txtBuscar, &QLineEdit::textChanged, this, &VentasForm::filtrarCatalogo);
    connect(ui.txtBuscar, &QLineEdit::returnPressed, this, &VentasForm::agregarPrimero);

    // Carrito y cobro
    connect(ui.btnCancelar, &QPushButton::clicked, this, &VentasForm::vaciar);
    connect(ui.btnDeshacer, &QPushButton::clicked, this, &VentasForm::deshacer);
    connect(ui.btnCobrar, &QPushButton::clicked, this, &VentasForm::cobrar);
    ui.spnPago->setLocale(QLocale(QLocale::Spanish, QLocale::CostaRica));
    connect(ui.spnPago, &QSpinBox::valueChanged, this, &VentasForm::mostrarPago);

    rapidos[0] = ui.btnRapido1;
    rapidos[1] = ui.btnRapido2;
    rapidos[2] = ui.btnRapido3;
    rapidos[3] = ui.btnRapido4;
    for (int i = 0; i < 4; i++) {
        montosRapidos[i] = 0;
        connect(rapidos[i], &QPushButton::clicked, this, [this, i]() { ponerPago(montosRapidos[i]); });
    }

    // Atajos de teclado
    QShortcut* buscar = new QShortcut(QKeySequence::Find, this);
    connect(buscar, &QShortcut::activated, this, [this]() {
        ui.txtBuscar->setFocus();
        ui.txtBuscar->selectAll();
    });
    QShortcut* cobrarAtajo = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Return), this);
    connect(cobrarAtajo, &QShortcut::activated, this, &VentasForm::cobrar);
    QShortcut* escape = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escape, &QShortcut::activated, this, &VentasForm::cerrarExito);

    QString teclaBuscar = QKeySequence(QKeySequence::Find).toString(QKeySequence::NativeText);
    QString teclaCobrar = QKeySequence(Qt::CTRL | Qt::Key_Return).toString(QKeySequence::NativeText);
    ui.txtBuscar->setPlaceholderText("Buscar producto   (" + teclaBuscar + ")");
    ui.lblAyuda->setText("Clic en un producto para agregarlo   ·   Enter en la búsqueda agrega el primero   ·   "
                         + teclaCobrar + " cobra");
    ui.btnCobrar->setToolTip("Cobrar (" + teclaCobrar + ")");

    // El espacio que sobra en el carrito es para las líneas (o el aviso de carrito vacío)
    ui.layoutCarrito->setStretchFactor(ui.lstCarrito, 1);
    ui.layoutCarrito->setStretchFactor(ui.lblVacio, 1);
    ui.barraDeshacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // Avisos que se esconden solos
    ui.barraDeshacer->hide();
    ui.lblError->hide();
    timerBarra.setSingleShot(true);
    timerBarra.setInterval(6000);
    connect(&timerBarra, &QTimer::timeout, ui.barraDeshacer, &QWidget::hide);
    timerError.setSingleShot(true);
    timerError.setInterval(4000);
    connect(&timerError, &QTimer::timeout, ui.lblError, &QWidget::hide);

    // Tarjeta de "venta lista": va encima del carrito y se esconde sola
    frmExito = new QFrame(ui.frmCarrito);
    frmExito->setObjectName("frmExito");
    QVBoxLayout* columna = new QVBoxLayout(frmExito);
    columna->setContentsMargins(24, 24, 24, 24);
    columna->setSpacing(10);
    columna->addStretch();
    lblCheque = new QLabel();
    lblCheque->setFixedSize(96, 96);
    lblExitoTitulo = new QLabel();
    lblExitoTitulo->setObjectName("lblExitoTitulo");
    lblExitoVuelto = new QLabel();
    lblExitoVuelto->setObjectName("lblExitoVuelto");
    lblExitoDetalle = new QLabel();
    lblExitoDetalle->setObjectName("lblExitoDetalle");
    lblExitoDetalle->setAlignment(Qt::AlignCenter);
    btnNuevaVenta = new QPushButton("Nueva venta");
    btnNuevaVenta->setObjectName("btnNuevaVenta");
    btnNuevaVenta->setCursor(Qt::PointingHandCursor);
    columna->addWidget(lblCheque, 0, Qt::AlignHCenter);
    columna->addWidget(lblExitoTitulo, 0, Qt::AlignHCenter);
    columna->addWidget(lblExitoVuelto, 0, Qt::AlignHCenter);
    columna->addWidget(lblExitoDetalle, 0, Qt::AlignHCenter);
    columna->addSpacing(8);
    columna->addWidget(btnNuevaVenta);
    columna->addStretch();
    frmExito->hide();
    connect(btnNuevaVenta, &QPushButton::clicked, this, &VentasForm::cerrarExito);
    timerExito.setSingleShot(true);
    timerExito.setInterval(7000);
    connect(&timerExito, &QTimer::timeout, this, &VentasForm::cerrarExito);

    llenarCatalogo();
    mostrarCarrito(-1);
}

// MainWindow la llama al cambiar de pestaña, por si cambiaron los productos
void VentasForm::actualizar()
{
    llenarCatalogo();
    mostrarCarrito(-1);
}

QString VentasForm::colones(int monto) const
{
    QLocale local(QLocale::Spanish, QLocale::CostaRica);
    return "₡" + local.toString(monto);
}

// ---------------------------------------------------------------- Catálogo

// Una tarjeta por producto activo, con su imagen, precio y cuántas quedan
void VentasForm::llenarCatalogo()
{
    ui.lstProductos->clear();
    for (int i = 0; i < inventario->getCantidad(); i++) {
        Producto p = inventario->getProducto(i);
        if (p.getActivo() == false) {
            continue;
        }
        int enCarrito = carrito.cantidadDe(p.getCodigo());
        int quedan = p.getExistencias() - enCarrito;

        QPixmap foto = Imagenes::cuadrada(p.getCodigo(), p.getNombre(), true, LADO_FOTO);
        QPainter pintor(&foto);
        pintor.setRenderHint(QPainter::Antialiasing);
        if (quedan <= 0) {
            // Sin existencias: la foto se ve apagada
            pintor.setCompositionMode(QPainter::CompositionMode_SourceAtop);
            QColor tapa = Estilo::color("fondo");
            tapa.setAlpha(170);
            pintor.fillRect(QRectF(0, 0, LADO_FOTO, LADO_FOTO), tapa);
            pintor.setCompositionMode(QPainter::CompositionMode_SourceOver);
        }
        if (enCarrito > 0) {
            // Globito azul con cuántos hay en el carrito
            pintor.setPen(QPen(QColor("#ffffff"), 2));
            pintor.setBrush(Estilo::color("acento"));
            pintor.drawEllipse(QPointF(LADO_FOTO - 15, 15), 13, 13);
            QFont letra = font();
            letra.setPixelSize(13);
            letra.setBold(true);
            pintor.setFont(letra);
            pintor.setPen(QColor("#ffffff"));
            pintor.drawText(QRectF(LADO_FOTO - 28, 2, 26, 26), Qt::AlignCenter, QString::number(enCarrito));
        }
        pintor.end();

        QString texto = p.getNombre() + "\n" + colones(p.getPrecio());
        if (quedan <= 0) {
            texto = texto + "\nSin existencias";
        } else if (quedan <= Ajustes::pocasExistencias()) {
            texto = texto + "\nQuedan " + QString::number(quedan);
        }

        QListWidgetItem* item = new QListWidgetItem(QIcon(foto), texto);
        item->setData(Qt::UserRole, p.getCodigo());
        item->setData(Qt::UserRole + 1, p.getCodigo() + " " + p.getNombre());
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignTop);
        if (quedan <= 0) {
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
            item->setToolTip("No quedan existencias de " + p.getNombre());
        } else {
            item->setToolTip("Agregar " + p.getNombre() + " · quedan " + QString::number(quedan));
        }
        ui.lstProductos->addItem(item);
    }
    filtrarCatalogo();
}

void VentasForm::filtrarCatalogo()
{
    QString texto = ui.txtBuscar->text().trimmed();
    for (int i = 0; i < ui.lstProductos->count(); i++) {
        QListWidgetItem* item = ui.lstProductos->item(i);
        bool coincide = texto == "" || item->data(Qt::UserRole + 1).toString().contains(texto, Qt::CaseInsensitive);
        item->setHidden(coincide == false);
    }
}

void VentasForm::agregarDesdeCatalogo(QListWidgetItem* item)
{
    cerrarExito();
    if ((item->flags() & Qt::ItemIsEnabled) == 0) {
        return;
    }
    animarVuelo(item);
    agregarProducto(item->data(Qt::UserRole).toString());
}

// Enter en la búsqueda: agrega el primer producto que se ve y deja lista la búsqueda para el siguiente
void VentasForm::agregarPrimero()
{
    for (int i = 0; i < ui.lstProductos->count(); i++) {
        QListWidgetItem* item = ui.lstProductos->item(i);
        if (item->isHidden() == false && (item->flags() & Qt::ItemIsEnabled) != 0) {
            agregarDesdeCatalogo(item);
            ui.txtBuscar->clear();
            return;
        }
    }
    mostrarError("No hay un producto con ese nombre para agregar.");
}

// La foto del producto "vuela" de la tarjeta al carrito
void VentasForm::animarVuelo(QListWidgetItem* item)
{
    if (Ajustes::animaciones() == false) {
        return;
    }
    QRect tarjeta = ui.lstProductos->visualItemRect(item);
    if (tarjeta.isEmpty() == true) {
        return;
    }
    QPoint inicio = ui.lstProductos->viewport()->mapTo(this, tarjeta.center());
    QPoint destino = ui.lblVentaTitulo->mapTo(this, QPoint(ui.lblVentaTitulo->width() / 2, ui.lblVentaTitulo->height() / 2));

    QString codigo = item->data(Qt::UserRole).toString();
    QString nombre = "";
    if (inventario->buscar(codigo) != -1) {
        nombre = inventario->getProducto(inventario->buscar(codigo)).getNombre();
    }
    QLabel* volador = new QLabel(this);
    volador->setPixmap(Imagenes::cuadrada(codigo, nombre, true, 56));
    volador->setScaledContents(true);
    volador->setGeometry(inicio.x() - 28, inicio.y() - 40, 56, 56);
    volador->show();
    volador->raise();

    Estilo::terminarEfectosArriba(volador);
    QGraphicsOpacityEffect* efecto = new QGraphicsOpacityEffect(volador);
    volador->setGraphicsEffect(efecto);

    QParallelAnimationGroup* grupo = new QParallelAnimationGroup(volador);
    QPropertyAnimation* movimiento = new QPropertyAnimation(volador, "geometry");
    movimiento->setDuration(520);
    movimiento->setStartValue(volador->geometry());
    movimiento->setEndValue(QRect(destino.x() - 10, destino.y() - 10, 20, 20));
    movimiento->setEasingCurve(QEasingCurve::InOutCubic);
    QPropertyAnimation* opacidad = new QPropertyAnimation(efecto, "opacity");
    opacidad->setDuration(520);
    opacidad->setKeyValueAt(0.0, 1.0);
    opacidad->setKeyValueAt(0.7, 0.9);
    opacidad->setKeyValueAt(1.0, 0.0);
    grupo->addAnimation(movimiento);
    grupo->addAnimation(opacidad);
    connect(grupo, &QParallelAnimationGroup::finished, volador, &QObject::deleteLater);
    grupo->start();
}

// ---------------------------------------------------------------- Carrito

void VentasForm::agregarProducto(QString codigo)
{
    int posicion = inventario->buscar(codigo);
    if (posicion == -1) {
        return;
    }
    QString error = carrito.agregar(inventario->getProducto(posicion), 1);
    if (error != "") {
        mostrarError(error);
        return;
    }
    int linea = -1;
    for (int i = 0; i < carrito.getCantidad(); i++) {
        if (carrito.getLinea(i).getCodigo() == codigo) {
            linea = i;
        }
    }
    mostrarCarrito(linea);
    llenarCatalogo();
}

void VentasForm::cambiarCantidad(int posicion, int cantidad)
{
    if (posicion < 0 || posicion >= carrito.getCantidad()) {
        return;
    }
    if (cantidad <= 0) {
        quitarLinea(posicion);
        return;
    }
    LineaVenta linea = carrito.getLinea(posicion);
    int existencias = 0;
    int enInventario = inventario->buscar(linea.getCodigo());
    if (enInventario != -1) {
        existencias = inventario->getProducto(enInventario).getExistencias();
    }
    QString error = carrito.cambiarCantidad(posicion, cantidad, existencias);
    if (error != "") {
        mostrarError(error);
        return;
    }
    mostrarCarrito(posicion);
    llenarCatalogo();
}

// Quitar una línea se puede deshacer
void VentasForm::quitarLinea(int posicion)
{
    if (posicion < 0 || posicion >= carrito.getCantidad()) {
        return;
    }
    QString nombre = carrito.getLinea(posicion).getNombre();
    carritoAnterior = carrito;
    carrito.quitar(posicion);
    mostrarCarrito(-1);
    llenarCatalogo();
    mostrarBarra("Se quitó " + nombre);
}

// Vaciar la venta también se puede deshacer, en vez de preguntar "¿está seguro?"
void VentasForm::vaciar()
{
    if (carrito.getCantidad() == 0) {
        return;
    }
    int unidades = carrito.getUnidades();
    carritoAnterior = carrito;
    carrito.vaciar();
    ui.spnPago->setValue(0);
    mostrarCarrito(-1);
    llenarCatalogo();
    if (unidades == 1) {
        mostrarBarra("Se vació la venta (1 artículo)");
    } else {
        mostrarBarra("Se vació la venta (" + QString::number(unidades) + " artículos)");
    }
}

void VentasForm::deshacer()
{
    carrito = carritoAnterior;
    ui.barraDeshacer->hide();
    mostrarCarrito(-1);
    llenarCatalogo();
}

// Vuelve a dibujar las líneas del carrito. "resaltar" es la línea que se acaba de cambiar.
void VentasForm::mostrarCarrito(int resaltar)
{
    ui.lstCarrito->clear();
    for (int i = 0; i < carrito.getCantidad(); i++) {
        QListWidgetItem* item = new QListWidgetItem(ui.lstCarrito);
        item->setSizeHint(QSize(0, 60));
        ui.lstCarrito->setItemWidget(item, crearFila(i));
    }

    bool vacio = carrito.getCantidad() == 0;
    ui.lstCarrito->setVisible(vacio == false);
    ui.lblVacio->setVisible(vacio);
    ui.btnCancelar->setVisible(vacio == false);
    int unidades = carrito.getUnidades();
    if (unidades == 0) {
        ui.lblArticulos->clear();
    } else if (unidades == 1) {
        ui.lblArticulos->setText("·  1 artículo");
    } else {
        ui.lblArticulos->setText("·  " + QString::number(unidades) + " artículos");
    }

    // La línea que cambió se ilumina un momento en azul
    if (resaltar >= 0 && resaltar < ui.lstCarrito->count()) {
        ui.lstCarrito->scrollToItem(ui.lstCarrito->item(resaltar));
        QWidget* fila = ui.lstCarrito->itemWidget(ui.lstCarrito->item(resaltar));
        QVariantAnimation* brillo = new QVariantAnimation(fila);
        brillo->setDuration(700);
        brillo->setStartValue(1.0);
        brillo->setEndValue(0.0);
        brillo->setEasingCurve(QEasingCurve::OutCubic);
        connect(brillo, &QVariantAnimation::valueChanged, fila, [fila](const QVariant& valor) {
            int alfa = qRound(valor.toDouble() * 45);
            fila->setStyleSheet("QWidget#fila { background: rgba(0, 113, 227, " + QString::number(alfa) + "); border-radius: 10px; }");
        });
        brillo->start(QAbstractAnimation::DeleteWhenStopped);
    }
    mostrarTotal();
    mostrarPago();
}

// Una línea del carrito: foto, nombre, − cantidad +, subtotal y la × para quitarla
QWidget* VentasForm::crearFila(int posicion)
{
    LineaVenta linea = carrito.getLinea(posicion);

    QWidget* fila = new QWidget();
    fila->setObjectName("fila");
    fila->setAttribute(Qt::WA_StyledBackground, true);
    QHBoxLayout* horizontal = new QHBoxLayout(fila);
    horizontal->setContentsMargins(6, 4, 4, 4);
    horizontal->setSpacing(8);

    QLabel* foto = new QLabel();
    foto->setPixmap(Imagenes::cuadrada(linea.getCodigo(), linea.getNombre(), true, 40));
    foto->setFixedSize(40, 40);

    QVBoxLayout* textos = new QVBoxLayout();
    textos->setSpacing(0);
    QLabel* nombre = new QLabel(linea.getNombre());
    nombre->setObjectName("filaNombre");
    QLabel* precio = new QLabel(colones(linea.getPrecio()) + " c/u");
    precio->setObjectName("filaPrecio");
    textos->addWidget(nombre);
    textos->addWidget(precio);

    QPushButton* menos = new QPushButton("−");
    menos->setObjectName("filaMenos");
    QLabel* cantidad = new QLabel(QString::number(linea.getCantidad()));
    cantidad->setObjectName("filaCantidad");
    cantidad->setAlignment(Qt::AlignCenter);
    cantidad->setFixedWidth(26);
    QPushButton* mas = new QPushButton("+");
    mas->setObjectName("filaMas");
    QLabel* subtotal = new QLabel(colones(linea.getSubtotal()));
    subtotal->setObjectName("filaSubtotal");
    subtotal->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    subtotal->setMinimumWidth(70);
    QPushButton* quitar = new QPushButton("×");
    quitar->setObjectName("filaQuitar");
    quitar->setToolTip("Quitar de la venta");

    menos->setCursor(Qt::PointingHandCursor);
    mas->setCursor(Qt::PointingHandCursor);
    quitar->setCursor(Qt::PointingHandCursor);

    horizontal->addWidget(foto);
    horizontal->addLayout(textos, 1);
    horizontal->addWidget(menos);
    horizontal->addWidget(cantidad);
    horizontal->addWidget(mas);
    horizontal->addWidget(subtotal);
    horizontal->addWidget(quitar);

    // Los cambios se hacen un momento después, porque la fila se borra y se vuelve a crear
    int unidades = linea.getCantidad();
    connect(menos, &QPushButton::clicked, this, [this, posicion, unidades]() {
        QTimer::singleShot(0, this, [this, posicion, unidades]() { cambiarCantidad(posicion, unidades - 1); });
    });
    connect(mas, &QPushButton::clicked, this, [this, posicion, unidades]() {
        QTimer::singleShot(0, this, [this, posicion, unidades]() { cambiarCantidad(posicion, unidades + 1); });
    });
    connect(quitar, &QPushButton::clicked, this, [this, posicion]() {
        QTimer::singleShot(0, this, [this, posicion]() { quitarLinea(posicion); });
    });
    return fila;
}

// ---------------------------------------------------------------- Cobro

// El total cambia contando hacia arriba o hacia abajo, y se arman los botones de pago rápido
void VentasForm::mostrarTotal()
{
    int total = carrito.getTotal();

    if (animacionTotal != nullptr) {
        animacionTotal->stop();
    }
    if (total != totalMostrado && Ajustes::animaciones() == true) {
        QVariantAnimation* animacion = new QVariantAnimation(this);
        animacionTotal = animacion;
        animacion->setDuration(380);
        animacion->setStartValue(totalMostrado);
        animacion->setEndValue(total);
        animacion->setEasingCurve(QEasingCurve::OutCubic);
        connect(animacion, &QVariantAnimation::valueChanged, this, [this](const QVariant& valor) {
            ui.lblTotal->setText(colones(valor.toInt()));
        });
        animacion->start(QAbstractAnimation::DeleteWhenStopped);
        totalMostrado = total;
    } else {
        ui.lblTotal->setText(colones(total));
        totalMostrado = total;
    }

    // Pago rápido: el monto exacto y los billetes que más se usan para pagar ese total
    for (int i = 0; i < 4; i++) {
        montosRapidos[i] = 0;
    }
    if (total > 0) {
        montosRapidos[0] = total;
        int billetes[6] = {500, 1000, 2000, 5000, 10000, 20000};
        int cuantos = 1;
        for (int i = 0; i < 6 && cuantos < 4; i++) {
            int redondeado = ((total + billetes[i] - 1) / billetes[i]) * billetes[i];
            bool repetido = false;
            for (int j = 0; j < cuantos; j++) {
                if (montosRapidos[j] == redondeado) {
                    repetido = true;
                }
            }
            if (redondeado > total && repetido == false) {
                montosRapidos[cuantos] = redondeado;
                cuantos++;
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        rapidos[i]->setVisible(montosRapidos[i] > 0);
        if (i == 0) {
            rapidos[i]->setText("Exacto");
        } else {
            rapidos[i]->setText(colones(montosRapidos[i]));
        }
    }
}

// Vuelto o lo que falta, y el botón Cobrar solo se activa cuando el pago alcanza
void VentasForm::mostrarPago()
{
    int total = carrito.getTotal();
    int pago = ui.spnPago->value();
    QString estado = "nada";

    if (total == 0) {
        ui.lblVuelto->clear();
        ui.btnCobrar->setText("Cobrar");
        ui.btnCobrar->setEnabled(false);
    } else {
        ui.btnCobrar->setText("Cobrar " + colones(total));
        if (pago == 0) {
            ui.lblVuelto->setText("¿Con cuánto paga? Toca un monto o escríbelo.");
            ui.btnCobrar->setEnabled(false);
        } else if (pago < total) {
            ui.lblVuelto->setText("Faltan " + colones(total - pago));
            ui.btnCobrar->setEnabled(false);
            estado = "falta";
        } else if (pago == total) {
            ui.lblVuelto->setText("Pago exacto, no hay vuelto");
            ui.btnCobrar->setEnabled(true);
            estado = "vuelto";
        } else {
            ui.lblVuelto->setText("Vuelto  " + colones(pago - total));
            ui.btnCobrar->setEnabled(true);
            estado = "vuelto";
        }
    }
    ui.lblVuelto->setProperty("estado", estado);
    ui.lblVuelto->style()->unpolish(ui.lblVuelto);
    ui.lblVuelto->style()->polish(ui.lblVuelto);

    // Se marca el botón rápido que coincide con el pago
    for (int i = 0; i < 4; i++) {
        bool elegido = pago > 0 && montosRapidos[i] == pago;
        if (rapidos[i]->property("elegido").toBool() != elegido) {
            rapidos[i]->setProperty("elegido", elegido);
            rapidos[i]->style()->unpolish(rapidos[i]);
            rapidos[i]->style()->polish(rapidos[i]);
        }
    }
}

void VentasForm::ponerPago(int monto)
{
    ui.spnPago->setValue(monto);
}

void VentasForm::cobrar()
{
    if (frmExito->isVisible() == true) {
        return;
    }
    if (carrito.getCantidad() == 0) {
        mostrarError("Agrega al menos un producto a la venta.");
        return;
    }
    int pago = ui.spnPago->value();
    if (pago < carrito.getTotal()) {
        mostrarError("El pago no alcanza: faltan " + colones(carrito.getTotal() - pago) + ".");
        ui.spnPago->setFocus();
        return;
    }

    QString error = carrito.cobrar(pago, inventario, historial);
    if (error != "") {
        mostrarError(error);
        return;
    }

    Venta venta = historial->getVenta(historial->getCantidad() - 1);

    // El inventario y el historial ya cambiaron: se guardan para que no se pierdan al cerrar
    if (ArchivoProductos::guardar(inventario) == false) {
        mostrarError("No se pudo guardar productos.json");
    }
    if (ArchivoVentas::guardar(historial) == false) {
        mostrarError("No se pudo guardar ventas.json");
    }
    TicketTxt ticket;
    if (ticket.guardar(venta) == false) {
        mostrarError("No se pudo guardar el ticket.");
    }

    ui.spnPago->setValue(0);
    ui.barraDeshacer->hide();
    mostrarCarrito(-1);
    llenarCatalogo();
    mostrarExito(venta);
}

// Tarjeta grande con el vuelto, como la pantalla final de Apple Pay
void VentasForm::mostrarExito(Venta venta)
{
    lblExitoTitulo->setText("Venta #" + QString::number(venta.getNumero()) + " lista");
    if (venta.getCambio() == 0) {
        lblExitoVuelto->setText("Pago exacto");
    } else {
        lblExitoVuelto->setText("Vuelto " + colones(venta.getCambio()));
    }
    lblExitoDetalle->setText("Total " + colones(venta.getTotal()) + "   ·   Pagó " + colones(venta.getPago())
                             + "\nTicket guardado como ticket-" + QString::number(venta.getNumero()) + ".txt");

    frmExito->setGeometry(ui.frmCarrito->rect());
    frmExito->raise();
    frmExito->show();
    btnNuevaVenta->setFocus();
    timerExito.start();

    if (Ajustes::animaciones() == false) {
        lblCheque->setPixmap(dibujarCheque(1.0));
        return;
    }
    Estilo::terminarEfectosArriba(frmExito);
    QGraphicsOpacityEffect* efecto = new QGraphicsOpacityEffect(frmExito);
    frmExito->setGraphicsEffect(efecto);
    QVariantAnimation* animacion = new QVariantAnimation(frmExito);
    animacion->setDuration(560);
    animacion->setStartValue(0.0);
    animacion->setEndValue(1.0);
    QPointer<QGraphicsOpacityEffect> cuidado = efecto;
    connect(animacion, &QVariantAnimation::valueChanged, this, [this, cuidado](const QVariant& valor) {
        double avance = valor.toDouble();
        if (cuidado != nullptr) {
            cuidado->setOpacity(qMin(1.0, avance * 2.5));
        }
        // El círculo crece con un pequeño rebote
        QEasingCurve rebote(QEasingCurve::OutBack);
        lblCheque->setPixmap(dibujarCheque(rebote.valueForProgress(avance)));
    });
    connect(animacion, &QVariantAnimation::finished, this, [this]() {
        frmExito->setGraphicsEffect(nullptr);
    });
    animacion->start(QAbstractAnimation::DeleteWhenStopped);
}

void VentasForm::cerrarExito()
{
    if (frmExito->isVisible() == false) {
        return;
    }
    timerExito.stop();
    frmExito->hide();
    ui.txtBuscar->setFocus();
}

// ---------------------------------------------------------------- Avisos

void VentasForm::mostrarError(QString error)
{
    ui.lblError->setText(error);
    ui.lblError->show();
    timerError.start();
}

void VentasForm::mostrarBarra(QString texto)
{
    ui.lblDeshacer->setText(texto);
    ui.barraDeshacer->show();
    timerBarra.start();
}

// La tarjeta de venta lista siempre tapa todo el carrito
void VentasForm::resizeEvent(QResizeEvent* evento)
{
    QWidget::resizeEvent(evento);
    QTimer::singleShot(0, this, [this]() {
        frmExito->setGeometry(ui.frmCarrito->rect());
    });
}
