#include "ProductosForm.h"
#include "ArchivoProductos.h"
#include <QShortcut>
#include <QKeySequence>
#include <QHeaderView>
#include <QLocale>
#include <QColor>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QStringList>
#include <QGraphicsOpacityEffect>
#include <QVariantAnimation>
#include <QEasingCurve>

// Desde esta cantidad para abajo un producto se marca como "por agotarse"
const int POCAS_EXISTENCIAS = 5;

// Columnas de la tabla
const int COL_CODIGO = 0;
const int COL_NOMBRE = 1;
const int COL_PRECIO = 2;
const int COL_EXISTENCIAS = 3;
const int COL_ESTADO = 4;

ProductosForm::ProductosForm(Inventario* inventario, QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->inventario = inventario;
    posicionActual = -1;
    llenando = false;

    // Tabla: al elegir una fila (clic o teclado) se carga en la ficha
    connect(ui.tablaProductos, &QTableWidget::cellClicked, this, &ProductosForm::seleccionarProducto);
    connect(ui.tablaProductos, &QTableWidget::currentCellChanged, this, &ProductosForm::seleccionarProducto);
    // Doble clic en precio o existencias para cambiarlos ahí mismo
    connect(ui.tablaProductos, &QTableWidget::itemChanged, this, &ProductosForm::editarEnTabla);

    // Búsqueda y filtros
    connect(ui.txtBuscar, &QLineEdit::textChanged, this, &ProductosForm::aplicarFiltro);
    connect(ui.btnTodos, &QPushButton::clicked, this, &ProductosForm::aplicarFiltro);
    connect(ui.btnActivos, &QPushButton::clicked, this, &ProductosForm::aplicarFiltro);
    connect(ui.btnPocas, &QPushButton::clicked, this, &ProductosForm::aplicarFiltro);
    connect(ui.btnInactivos, &QPushButton::clicked, this, &ProductosForm::aplicarFiltro);

    // Ficha
    connect(ui.btnNuevo, &QPushButton::clicked, this, &ProductosForm::nuevoProducto);
    connect(ui.btnGuardar, &QPushButton::clicked, this, &ProductosForm::guardarProducto);
    connect(ui.btnDesactivar, &QPushButton::clicked, this, &ProductosForm::cambiarEstado);
    connect(ui.btnRecibir, &QPushButton::clicked, this, &ProductosForm::recibirMercaderia);
    connect(ui.txtCodigo, &QLineEdit::returnPressed, this, &ProductosForm::guardarProducto);
    connect(ui.txtNombre, &QLineEdit::returnPressed, this, &ProductosForm::guardarProducto);
    // En un producto nuevo la imagen muestra las iniciales mientras se escribe el nombre
    connect(ui.txtNombre, &QLineEdit::textChanged, this, [this]() {
        if (posicionActual == -1) {
            mostrarImagen(false);
        }
    });

    // Atajos de teclado
    QShortcut* buscar = new QShortcut(QKeySequence::Find, this);
    connect(buscar, &QShortcut::activated, this, [this]() {
        ui.txtBuscar->setFocus();
        ui.txtBuscar->selectAll();
    });
    QShortcut* nuevo = new QShortcut(QKeySequence::New, this);
    connect(nuevo, &QShortcut::activated, this, &ProductosForm::nuevoProducto);
    QShortcut* guardar = new QShortcut(QKeySequence::Save, this);
    connect(guardar, &QShortcut::activated, this, &ProductosForm::guardarProducto);

    // Los atajos se ven en la búsqueda y al pasar el mouse por los botones
    QString teclaBuscar = QKeySequence(QKeySequence::Find).toString(QKeySequence::NativeText);
    ui.txtBuscar->setPlaceholderText("Buscar por código o nombre   (" + teclaBuscar + ")");
    ui.btnNuevo->setToolTip("Nuevo producto (" + QKeySequence(QKeySequence::New).toString(QKeySequence::NativeText) + ")");
    ui.btnGuardar->setToolTip("Guardar (" + QKeySequence(QKeySequence::Save).toString(QKeySequence::NativeText) + ")");
    ui.tablaProductos->setToolTip("Doble clic en precio o existencias para cambiarlos ahí mismo");

    // El mensaje de la ficha se borra solo
    timerMensaje.setSingleShot(true);
    timerMensaje.setInterval(4000);
    connect(&timerMensaje, &QTimer::timeout, ui.lblMensaje, &QLabel::clear);

    // El nombre usa el espacio que sobra y las otras columnas se ajustan a su contenido
    QHeaderView* encabezado = ui.tablaProductos->horizontalHeader();
    encabezado->setSectionResizeMode(QHeaderView::ResizeToContents);
    encabezado->setSectionResizeMode(COL_NOMBRE, QHeaderView::Stretch);
    encabezado->setStretchLastSection(false);
    ui.tablaProductos->sortByColumn(COL_CODIGO, Qt::AscendingOrder);
    actualizar();
    nuevoProducto();
    ui.lblMensaje->clear();
}

// Vuelve a llenar la tabla con los productos del inventario
void ProductosForm::actualizar()
{
    llenarTabla();
    aplicarFiltro();
    mostrarResumen();
    mostrarFicha();
    mostrarImagen(false);
}

void ProductosForm::llenarTabla()
{
    llenando = true;
    ui.tablaProductos->setSortingEnabled(false);
    ui.tablaProductos->clearContents();
    ui.tablaProductos->setRowCount(inventario->getCantidad());

    for (int i = 0; i < inventario->getCantidad(); i++) {
        Producto p = inventario->getProducto(i);

        QTableWidgetItem* codigo = new QTableWidgetItem(p.getCodigo());
        codigo->setData(Qt::UserRole, i);   // la posición real en el inventario
        codigo->setFlags(codigo->flags() & ~Qt::ItemIsEditable);

        QTableWidgetItem* nombre = new QTableWidgetItem(p.getNombre());
        nombre->setFlags(nombre->flags() & ~Qt::ItemIsEditable);

        // Precio y existencias se guardan como número: así se ordenan bien
        // y al editarlos Qt muestra un campo de números
        QTableWidgetItem* precio = new QTableWidgetItem();
        precio->setData(Qt::DisplayRole, p.getPrecio());

        QTableWidgetItem* existencias = new QTableWidgetItem();
        existencias->setData(Qt::DisplayRole, p.getExistencias());

        QString textoEstado = "Activo";
        QColor colorEstado("#1a7f37");
        if (p.getActivo() == false) {
            textoEstado = "Inactivo";
            colorEstado = QColor("#8e8e93");
        } else if (p.getExistencias() == 0) {
            textoEstado = "Agotado";
            colorEstado = QColor("#d70015");
        } else if (p.getExistencias() <= POCAS_EXISTENCIAS) {
            textoEstado = "Por agotarse";
            colorEstado = QColor("#c93400");
        }
        QTableWidgetItem* estado = new QTableWidgetItem("● " + textoEstado);
        estado->setForeground(colorEstado);
        estado->setFlags(estado->flags() & ~Qt::ItemIsEditable);

        if (p.getActivo() == true && p.getExistencias() <= POCAS_EXISTENCIAS) {
            existencias->setForeground(colorEstado);
        }
        if (p.getActivo() == false) {
            codigo->setForeground(QColor("#8e8e93"));
            nombre->setForeground(QColor("#8e8e93"));
            precio->setForeground(QColor("#8e8e93"));
            existencias->setForeground(QColor("#8e8e93"));
        }

        ui.tablaProductos->setItem(i, COL_CODIGO, codigo);
        ui.tablaProductos->setItem(i, COL_NOMBRE, nombre);
        ui.tablaProductos->setItem(i, COL_PRECIO, precio);
        ui.tablaProductos->setItem(i, COL_EXISTENCIAS, existencias);
        ui.tablaProductos->setItem(i, COL_ESTADO, estado);
    }

    ui.tablaProductos->setSortingEnabled(true);

    // Volver a marcar el producto que se estaba editando
    ui.tablaProductos->blockSignals(true);
    ui.tablaProductos->clearSelection();
    for (int fila = 0; fila < ui.tablaProductos->rowCount(); fila++) {
        if (posicionDeFila(fila) == posicionActual) {
            ui.tablaProductos->setCurrentCell(fila, COL_NOMBRE);
            ui.tablaProductos->selectRow(fila);
        }
    }
    ui.tablaProductos->blockSignals(false);
    llenando = false;
}

// La tabla se puede ordenar, así que la fila no siempre es la posición del inventario
int ProductosForm::posicionDeFila(int fila) const
{
    QTableWidgetItem* item = ui.tablaProductos->item(fila, COL_CODIGO);
    if (item == nullptr) {
        return -1;
    }
    return item->data(Qt::UserRole).toInt();
}

// Oculta las filas que no coinciden con la búsqueda o con el filtro elegido
void ProductosForm::aplicarFiltro()
{
    QString texto = ui.txtBuscar->text().trimmed();
    int visibles = 0;

    for (int fila = 0; fila < ui.tablaProductos->rowCount(); fila++) {
        int posicion = posicionDeFila(fila);
        if (posicion == -1) {
            continue;
        }
        Producto p = inventario->getProducto(posicion);

        bool coincide = texto == "" || p.getCodigo().contains(texto, Qt::CaseInsensitive)
                        || p.getNombre().contains(texto, Qt::CaseInsensitive);

        bool pasaFiltro = true;
        if (ui.btnActivos->isChecked() == true) {
            pasaFiltro = p.getActivo() == true;
        } else if (ui.btnInactivos->isChecked() == true) {
            pasaFiltro = p.getActivo() == false;
        } else if (ui.btnPocas->isChecked() == true) {
            pasaFiltro = p.getActivo() == true && p.getExistencias() <= POCAS_EXISTENCIAS;
        }

        bool mostrar = coincide && pasaFiltro;
        ui.tablaProductos->setRowHidden(fila, mostrar == false);
        if (mostrar == true) {
            visibles++;
        }
    }
    mostrarResumen();
    if (visibles < ui.tablaProductos->rowCount()) {
        ui.lblResumen->setText("Mostrando " + QString::number(visibles) + " de "
                               + QString::number(ui.tablaProductos->rowCount()) + " productos");
    }
}

// Una línea abajo de la tabla con lo más importante del inventario
void ProductosForm::mostrarResumen()
{
    int activos = 0;
    int pocas = 0;
    int valor = 0;
    for (int i = 0; i < inventario->getCantidad(); i++) {
        Producto p = inventario->getProducto(i);
        if (p.getActivo() == true) {
            activos++;
            valor = valor + p.getPrecio() * p.getExistencias();
            if (p.getExistencias() <= POCAS_EXISTENCIAS) {
                pocas++;
            }
        }
    }
    QLocale local(QLocale::Spanish, QLocale::CostaRica);
    ui.lblResumen->setText(QString::number(inventario->getCantidad()) + " productos   ·   "
                           + QString::number(activos) + " activos   ·   "
                           + QString::number(pocas) + " por agotarse   ·   "
                           + "Valor del inventario ₡" + local.toString(valor));
    ui.btnPocas->setText("Por agotarse (" + QString::number(pocas) + ")");
}

// Al elegir una fila, ese producto pasa a la ficha de la derecha
void ProductosForm::seleccionarProducto(int fila, int columna)
{
    if (fila == -1 || llenando == true) {
        return;   // la tabla se quedó sin fila seleccionada o se está llenando
    }
    int posicion = posicionDeFila(fila);
    if (posicion == -1) {
        return;
    }
    bool esOtro = posicion != posicionActual;
    if (esOtro == true) {
        ui.lblMensaje->clear();
    }
    posicionActual = posicion;
    cargarCampos();
    mostrarFicha();
    mostrarImagen(esOtro);
}

// Pone la ficha en modo "nuevo" o "editar" según lo que esté elegido
void ProductosForm::mostrarFicha()
{
    bool editando = posicionActual != -1;
    ui.btnDesactivar->setVisible(editando);
    ui.lblRecibir->setVisible(editando);
    ui.spnRecibir->setVisible(editando);
    ui.btnRecibir->setVisible(editando);

    if (editando == false) {
        ui.lblTituloFicha->setText("Nuevo producto");
        ui.lblSubtitulo->setText("Llena los datos y presiona Agregar. El código se sugiere solo.");
        ui.txtCodigo->setReadOnly(false);
        ui.btnGuardar->setText("Agregar producto");
        return;
    }

    Producto p = inventario->getProducto(posicionActual);
    ui.lblTituloFicha->setText(p.getNombre());
    QString estado = "Activo";
    if (p.getActivo() == false) {
        estado = "Inactivo: no aparece en Vender";
    }
    ui.lblSubtitulo->setText(p.getCodigo() + "   ·   " + estado);
    ui.txtCodigo->setReadOnly(true);   // el código no se cambia después de creado
    ui.btnGuardar->setText("Guardar cambios");
    if (p.getActivo() == true) {
        ui.btnDesactivar->setText("Desactivar");
    } else {
        ui.btnDesactivar->setText("Activar de nuevo");
    }
}

// Copia los datos del producto elegido a los campos de la ficha.
// Va aparte de mostrarFicha para no borrar lo que se está escribiendo al cambiar de pestaña.
void ProductosForm::cargarCampos()
{
    if (posicionActual == -1) {
        return;
    }
    Producto p = inventario->getProducto(posicionActual);
    ui.txtCodigo->setText(p.getCodigo());
    ui.txtNombre->setText(p.getNombre());
    ui.spnPrecio->setValue(p.getPrecio());
    ui.spnExistencias->setValue(p.getExistencias());
}

// Limpia la ficha para agregar un producto, con el siguiente código ya puesto
void ProductosForm::nuevoProducto()
{
    posicionActual = -1;
    ui.tablaProductos->blockSignals(true);
    ui.tablaProductos->clearSelection();
    ui.tablaProductos->setCurrentCell(-1, -1);
    ui.tablaProductos->blockSignals(false);

    ui.txtCodigo->setText(siguienteCodigo());
    ui.txtNombre->clear();
    ui.spnPrecio->setValue(0);
    ui.spnExistencias->setValue(0);
    mostrarFicha();
    mostrarImagen(true);
    ui.txtNombre->setFocus();
}

// Busca el número más alto de los códigos tipo P001 y devuelve el siguiente
QString ProductosForm::siguienteCodigo() const
{
    int mayor = 0;
    for (int i = 0; i < inventario->getCantidad(); i++) {
        QString codigo = inventario->getProducto(i).getCodigo();
        if (codigo.startsWith("P") == true) {
            bool esNumero = false;
            int numero = codigo.mid(1).toInt(&esNumero);
            if (esNumero == true && numero > mayor) {
                mayor = numero;
            }
        }
    }
    return "P" + QString::number(mayor + 1).rightJustified(3, '0');
}

// Un solo botón: agrega si es nuevo o guarda los cambios si se está editando
void ProductosForm::guardarProducto()
{
    QString error;
    QString nombre = ui.txtNombre->text().trimmed();

    if (posicionActual == -1) {
        Producto nuevo(ui.txtCodigo->text().trimmed(), nombre, ui.spnPrecio->value(), ui.spnExistencias->value());
        error = inventario->agregar(nuevo);
        if (error != "") {
            mostrarMensaje(error, true);
            enfocarCampo(error);
            return;
        }
        if (guardarArchivo() == true) {
            // Queda lista para agregar otro, que es lo más común al cargar mercadería nueva
            QString codigo = nuevo.getCodigo();
            nuevoProducto();
            actualizar();
            mostrarMensaje("Se agregó " + nombre + " (" + codigo + "). Ya puedes agregar otro.", false);
        }
        return;
    }

    error = inventario->modificar(posicionActual, nombre, ui.spnPrecio->value(), ui.spnExistencias->value());
    if (error != "") {
        mostrarMensaje(error, true);
        enfocarCampo(error);
        return;
    }
    if (guardarArchivo() == true) {
        actualizar();
        mostrarMensaje("Cambios guardados.", false);
    }
}

void ProductosForm::cambiarEstado()
{
    if (posicionActual == -1) {
        return;
    }
    inventario->cambiarEstado(posicionActual);
    if (guardarArchivo() == true) {
        actualizar();
        Producto p = inventario->getProducto(posicionActual);
        if (p.getActivo() == true) {
            mostrarMensaje(p.getNombre() + " vuelve a aparecer en Vender.", false);
        } else {
            mostrarMensaje(p.getNombre() + " ya no aparece en Vender. Puedes activarlo cuando quieras.", false);
        }
    }
}

// Suma lo que llegó del proveedor sin tener que hacer la cuenta a mano
void ProductosForm::recibirMercaderia()
{
    if (posicionActual == -1) {
        return;
    }
    Producto p = inventario->getProducto(posicionActual);
    int llegaron = ui.spnRecibir->value();
    int nuevas = p.getExistencias() + llegaron;

    QString error = inventario->modificar(posicionActual, p.getNombre(), p.getPrecio(), nuevas);
    if (error != "") {
        mostrarMensaje(error, true);
        return;
    }
    if (guardarArchivo() == true) {
        ui.spnRecibir->setValue(1);
        cargarCampos();
        actualizar();
        mostrarMensaje("Se sumaron " + QString::number(llegaron) + " a " + p.getNombre()
                           + ". Ahora hay " + QString::number(nuevas) + ".", false);
    }
}

// Cuando se cambia el precio o las existencias directo en la tabla
void ProductosForm::editarEnTabla(QTableWidgetItem* item)
{
    if (llenando == true) {
        return;
    }
    int columna = item->column();
    if (columna != COL_PRECIO && columna != COL_EXISTENCIAS) {
        return;
    }
    int posicion = posicionDeFila(item->row());
    if (posicion == -1) {
        return;
    }
    Producto p = inventario->getProducto(posicion);
    int precio = p.getPrecio();
    int existencias = p.getExistencias();
    if (columna == COL_PRECIO) {
        precio = item->data(Qt::DisplayRole).toInt();
    } else {
        existencias = item->data(Qt::DisplayRole).toInt();
    }

    QString error = inventario->modificar(posicion, p.getNombre(), precio, existencias);
    posicionActual = posicion;
    cargarCampos();
    if (error != "") {
        mostrarMensaje(error, true);
    } else if (guardarArchivo() == true) {
        if (columna == COL_PRECIO) {
            mostrarMensaje("Nuevo precio de " + p.getNombre() + ": ₡" + QString::number(precio) + ".", false);
        } else {
            mostrarMensaje(p.getNombre() + " ahora tiene " + QString::number(existencias) + ".", false);
        }
    }
    // Se vuelve a llenar la tabla después, cuando Qt termina de cerrar el campo de edición
    QTimer::singleShot(0, this, &ProductosForm::actualizar);
}

// Mensaje debajo de los campos en vez de una ventana emergente
void ProductosForm::mostrarMensaje(QString texto, bool esError)
{
    if (esError == true) {
        ui.lblMensaje->setStyleSheet("color: #d70015;");
    } else {
        ui.lblMensaje->setStyleSheet("color: #1a7f37;");
    }
    ui.lblMensaje->setText(texto);
    timerMensaje.start();
}

// Pone el cursor en el campo que tiene el error
void ProductosForm::enfocarCampo(QString error)
{
    if (error.contains("código") == true) {
        ui.txtCodigo->setFocus();
        ui.txtCodigo->selectAll();
    } else if (error.contains("nombre") == true) {
        ui.txtNombre->setFocus();
        ui.txtNombre->selectAll();
    } else if (error.contains("precio") == true) {
        ui.spnPrecio->setFocus();
        ui.spnPrecio->selectAll();
    } else if (error.contains("existencias") == true) {
        ui.spnExistencias->setFocus();
        ui.spnExistencias->selectAll();
    }
}

bool ProductosForm::guardarArchivo()
{
    if (ArchivoProductos::guardar(inventario) == false) {
        mostrarMensaje("No se pudo guardar productos.txt", true);
        return false;
    }
    return true;
}

// Imagen del producto. Las de los productos de ejemplo están en imagenes/ con el código como nombre.
// Si un producto no tiene imagen, se dibuja una tarjeta con sus iniciales.
QPixmap ProductosForm::imagenProducto(QString codigo, QString nombre, bool activo) const
{
    QPixmap imagen(":/imagenes/" + codigo + ".png");

    if (imagen.isNull() == true) {
        // Tarjeta del mismo tamaño que las imágenes (760 x 340), con un color que sale del nombre
        imagen = QPixmap(760, 340);
        imagen.fill(Qt::transparent);
        int tono = qHash(nombre) % 360;

        QPainter pintor(&imagen);
        pintor.setRenderHint(QPainter::Antialiasing);
        QLinearGradient degradado(0, 0, 760, 340);
        degradado.setColorAt(0, QColor::fromHsl(tono, 160, 238));
        degradado.setColorAt(1, QColor::fromHsl(tono, 130, 205));
        pintor.setPen(Qt::NoPen);
        pintor.setBrush(degradado);
        pintor.drawRoundedRect(QRectF(0, 0, 760, 340), 28, 28);
        pintor.setBrush(QColor(255, 255, 255, 70));
        pintor.drawEllipse(QPointF(650, 50), 110, 110);
        pintor.drawEllipse(QPointF(90, 310), 80, 80);

        // Iniciales de las dos primeras palabras
        QString iniciales;
        // Solo cuentan las palabras de más de 2 letras, así "Azúcar 1 kg" da "AZ" y no "AK"
        QStringList palabras;
        QStringList todas = nombre.split(" ", Qt::SkipEmptyParts);
        for (int i = 0; i < todas.size(); i++) {
            if (todas[i].size() > 2 && todas[i][0].isLetter() == true) {
                palabras.append(todas[i]);
            }
        }
        if (palabras.size() == 1) {
            iniciales = palabras[0].left(2).toUpper();
        } else if (palabras.size() >= 2) {
            iniciales = QString(palabras[0][0].toUpper()) + palabras[1][0].toUpper();
        }
        if (iniciales == "") {
            iniciales = "+";
        }
        pintor.setBrush(QColor(255, 255, 255, 215));
        pintor.drawEllipse(QPointF(380, 170), 92, 92);
        QFont letra = font();
        letra.setPixelSize(70);
        letra.setBold(true);
        pintor.setFont(letra);
        pintor.setPen(QColor::fromHsl(tono, 110, 85));
        pintor.drawText(QRectF(288, 78, 184, 184), Qt::AlignCenter, iniciales);
        pintor.end();
    }

    // Un producto inactivo se ve deslavado
    if (activo == false) {
        QPainter pintor(&imagen);
        pintor.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        pintor.fillRect(imagen.rect(), QColor(245, 245, 247, 150));
        pintor.end();
    }
    return imagen;
}

// Cambia la imagen de la ficha. Con animación aparece suave y crece un poco, como en iOS.
void ProductosForm::mostrarImagen(bool animar)
{
    if (posicionActual == -1) {
        QString nombre = ui.txtNombre->text().trimmed();
        imagenActual = imagenProducto("", nombre, true);
    } else {
        Producto p = inventario->getProducto(posicionActual);
        imagenActual = imagenProducto(p.getCodigo(), p.getNombre(), p.getActivo());
    }

    if (animar == false) {
        pintarImagen(1.0);
        return;
    }

    // Si se hace clic rápido en varios productos, se corta la animación anterior
    if (animacionImagen != nullptr) {
        animacionImagen->stop();
    }

    QPointer<QGraphicsOpacityEffect> efecto = new QGraphicsOpacityEffect(ui.lblImagen);
    efecto->setOpacity(0.0);
    ui.lblImagen->setGraphicsEffect(efecto);

    QVariantAnimation* animacion = new QVariantAnimation(this);
    animacionImagen = animacion;
    animacion->setDuration(340);
    animacion->setStartValue(0.0);
    animacion->setEndValue(1.0);
    animacion->setEasingCurve(QEasingCurve::OutCubic);
    connect(animacion, &QVariantAnimation::valueChanged, this, [this, efecto](const QVariant& valor) {
        double avance = valor.toDouble();
        if (efecto != nullptr) {
            efecto->setOpacity(avance);
        }
        pintarImagen(0.92 + 0.08 * avance);
    });
    connect(animacion, &QVariantAnimation::finished, this, [this]() {
        ui.lblImagen->setGraphicsEffect(nullptr);
        pintarImagen(1.0);
    });
    animacion->start(QAbstractAnimation::DeleteWhenStopped);
}

// Dibuja la imagen actual en la etiqueta, del tamaño que cabe (escala 1.0 = tamaño normal)
void ProductosForm::pintarImagen(double escala)
{
    if (imagenActual.isNull() == true) {
        return;
    }
    int ancho = ui.lblImagen->width();
    if (ancho < 100) {
        ancho = 380;
    }
    int alto = ui.lblImagen->maximumHeight();
    double pantalla = devicePixelRatioF();
    QSize tamano(qRound(ancho * escala * pantalla), qRound(alto * escala * pantalla));
    QPixmap escalada = imagenActual.scaled(tamano, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    escalada.setDevicePixelRatio(pantalla);
    ui.lblImagen->setPixmap(escalada);
}

// Si cambia el tamaño de la ventana, la imagen se vuelve a ajustar
void ProductosForm::resizeEvent(QResizeEvent* evento)
{
    QWidget::resizeEvent(evento);
    pintarImagen(1.0);
}
