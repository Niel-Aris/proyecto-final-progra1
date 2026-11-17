#include "ProductosForm.h"
#include "ArchivoProductos.h"
#include <QShortcut>
#include <QKeySequence>
#include <QHeaderView>
#include <QLocale>
#include <QColor>
#include <QDate>
#include <QPainter>
#include <QLinearGradient>
#include <QFont>
#include <QStringList>
#include <QListWidgetItem>
#include <QStyle>
#include <QGraphicsOpacityEffect>
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

    // Lista: al elegir una fila (clic o teclado) se abre en la ficha
    connect(ui.tablaProductos, &QTableWidget::cellClicked, this, &ProductosForm::seleccionarProducto);
    connect(ui.tablaProductos, &QTableWidget::currentCellChanged, this, &ProductosForm::seleccionarProducto);
    connect(ui.tablaProductos, &QTableWidget::itemChanged, this, &ProductosForm::editarEnTabla);
    connect(ui.txtBuscar, &QLineEdit::textChanged, this, &ProductosForm::aplicarFiltro);
    connect(ui.btnTodos, &QPushButton::clicked, this, &ProductosForm::aplicarFiltro);
    connect(ui.btnActivos, &QPushButton::clicked, this, &ProductosForm::aplicarFiltro);
    connect(ui.btnPocas, &QPushButton::clicked, this, &ProductosForm::aplicarFiltro);
    connect(ui.btnInactivos, &QPushButton::clicked, this, &ProductosForm::aplicarFiltro);

    // Ficha: los campos se guardan solos al terminar de escribir (Enter o al salir del campo)
    connect(ui.txtNombre, &QLineEdit::editingFinished, this, &ProductosForm::guardarDatos);
    connect(ui.spnPrecio, &QSpinBox::editingFinished, this, &ProductosForm::guardarDatos);
    connect(ui.spnExistencias, &QSpinBox::editingFinished, this, &ProductosForm::guardarDatos);
    connect(ui.txtNombre, &QLineEdit::textEdited, this, [this]() {
        if (posicionActual == -1) {
            mostrarImagen(false);   // en un producto nuevo las iniciales cambian mientras se escribe
        }
    });
    connect(ui.txtNombre, &QLineEdit::returnPressed, this, [this]() {
        if (posicionActual == -1) {
            crearProducto();
        }
    });
    connect(ui.txtCodigo, &QLineEdit::returnPressed, this, &ProductosForm::crearProducto);

    // Botones de existencias: − y + de a uno, y los de mercadería que llega
    connect(ui.btnMenos, &QPushButton::clicked, this, [this]() { sumarExistencias(-1, "Ajuste"); });
    connect(ui.btnMas, &QPushButton::clicked, this, [this]() { sumarExistencias(1, "Ajuste"); });
    connect(ui.btnMas6, &QPushButton::clicked, this, [this]() { sumarExistencias(6, "Recibido"); });
    connect(ui.btnMas12, &QPushButton::clicked, this, [this]() { sumarExistencias(12, "Recibido"); });
    connect(ui.btnMas24, &QPushButton::clicked, this, [this]() { sumarExistencias(24, "Recibido"); });
    connect(ui.swActivo, &QAbstractButton::toggled, this, &ProductosForm::cambiarDisponible);

    connect(ui.btnNuevo, &QPushButton::clicked, this, &ProductosForm::nuevoProducto);
    connect(ui.btnCrear, &QPushButton::clicked, this, &ProductosForm::crearProducto);
    connect(ui.btnDeshacer, &QPushButton::clicked, this, &ProductosForm::deshacer);

    // Atajos de teclado
    QShortcut* buscar = new QShortcut(QKeySequence::Find, this);
    connect(buscar, &QShortcut::activated, this, [this]() {
        ui.txtBuscar->setFocus();
        ui.txtBuscar->selectAll();
    });
    QShortcut* nuevo = new QShortcut(QKeySequence::New, this);
    connect(nuevo, &QShortcut::activated, this, &ProductosForm::nuevoProducto);
    QShortcut* atras = new QShortcut(QKeySequence::Undo, this);
    connect(atras, &QShortcut::activated, this, &ProductosForm::deshacer);
    // Con la lista seleccionada, + y − suman o restan una unidad
    QShortcut* mas = new QShortcut(QKeySequence("+"), ui.tablaProductos, nullptr, nullptr, Qt::WidgetShortcut);
    connect(mas, &QShortcut::activated, this, [this]() { sumarExistencias(1, "Ajuste"); });
    QShortcut* igual = new QShortcut(QKeySequence("="), ui.tablaProductos, nullptr, nullptr, Qt::WidgetShortcut);
    connect(igual, &QShortcut::activated, this, [this]() { sumarExistencias(1, "Ajuste"); });
    QShortcut* menos = new QShortcut(QKeySequence("-"), ui.tablaProductos, nullptr, nullptr, Qt::WidgetShortcut);
    connect(menos, &QShortcut::activated, this, [this]() { sumarExistencias(-1, "Ajuste"); });

    QString teclaBuscar = QKeySequence(QKeySequence::Find).toString(QKeySequence::NativeText);
    QString teclaDeshacer = QKeySequence(QKeySequence::Undo).toString(QKeySequence::NativeText);
    ui.txtBuscar->setPlaceholderText("Buscar por código o nombre   (" + teclaBuscar + ")");
    ui.btnNuevo->setToolTip("Nuevo producto (" + QKeySequence(QKeySequence::New).toString(QKeySequence::NativeText) + ")");
    ui.btnDeshacer->setToolTip("Deshacer (" + teclaDeshacer + ")");
    ui.tablaProductos->setToolTip("Doble clic en precio o existencias para cambiarlos. Con la lista elegida, + y − suman o restan uno.");
    ui.btnMenos->setToolTip("Restar uno");
    ui.btnMas->setToolTip("Sumar uno");

    // La barra de "Deshacer" se esconde sola
    ui.barraDeshacer->hide();
    timerBarra.setSingleShot(true);
    timerBarra.setInterval(6000);
    connect(&timerBarra, &QTimer::timeout, ui.barraDeshacer, &QWidget::hide);

    // Columnas: el nombre usa el espacio que sobra, las demás se ajustan a lo que tienen
    QHeaderView* encabezado = ui.tablaProductos->horizontalHeader();
    encabezado->setSectionResizeMode(QHeaderView::ResizeToContents);
    encabezado->setSectionResizeMode(COL_NOMBRE, QHeaderView::Stretch);
    encabezado->setStretchLastSection(false);
    ui.tablaProductos->setIconSize(QSize(28, 28));
    ui.spnPrecio->setLocale(QLocale(QLocale::Spanish, QLocale::CostaRica));   // ₡1 300 como en el resumen
    ui.lblError->hide();
    // La actividad se lleva casi todo el espacio que sobra en la ficha
    ui.layoutFicha->setStretchFactor(ui.lstActividad, 3);
    ui.tablaProductos->sortByColumn(COL_CODIGO, Qt::AscendingOrder);

    actualizar();
    nuevoProducto();
}

// Vuelve a llenar la lista y la ficha con lo que tiene el inventario
void ProductosForm::actualizar()
{
    if (posicionActual >= inventario->getCantidad()) {
        posicionActual = -1;   // por ejemplo, si se deshizo la creación de un producto
    }
    llenarTabla();
    aplicarFiltro();
    mostrarFicha();
    mostrarActividad();
    mostrarImagen(false);
}

// ---------------------------------------------------------------- Lista

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
        nombre->setIcon(miniatura(p.getCodigo(), p.getNombre(), p.getActivo()));
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

    // Volver a marcar el producto que está abierto en la ficha
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

// Una línea abajo de la lista con lo más importante del inventario
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

// Al elegir una fila, ese producto se abre en la ficha
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
    posicionActual = posicion;
    if (esOtro == true) {
        limpiarError();
        cargarCampos();
        mostrarFicha();
        mostrarActividad();
        mostrarImagen(true);
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

    posicionActual = posicion;
    QString error = inventario->modificar(posicion, p.getNombre(), precio, existencias);
    if (error != "") {
        mostrarError(error);
    } else {
        cambioHecho();
    }
    cargarCampos();
    // Se vuelve a llenar la tabla cuando Qt termina de cerrar el campo de edición
    QTimer::singleShot(0, this, &ProductosForm::actualizar);
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

// ---------------------------------------------------------------- Ficha

// Copia los datos del producto abierto a los campos de la ficha
void ProductosForm::cargarCampos()
{
    if (posicionActual == -1) {
        return;
    }
    Producto p = inventario->getProducto(posicionActual);
    llenando = true;
    ui.txtNombre->setText(p.getNombre());
    ui.txtNombre->setCursorPosition(0);
    ui.txtCodigo->setText(p.getCodigo());
    ui.spnPrecio->setValue(p.getPrecio());
    ui.spnExistencias->setValue(p.getExistencias());
    ui.swActivo->blockSignals(true);
    ui.swActivo->setChecked(p.getActivo());
    ui.swActivo->blockSignals(false);
    ui.swActivo->update();
    llenando = false;
}

// Muestra u oculta las partes de la ficha según si el producto es nuevo o ya existe
void ProductosForm::mostrarFicha()
{
    bool editando = posicionActual != -1;
    ui.lblSubtitulo->setVisible(editando);
    ui.lblCodigo->setVisible(editando == false);
    ui.txtCodigo->setVisible(editando == false);
    ui.lblRecibir->setVisible(editando);
    ui.btnMas6->setVisible(editando);
    ui.btnMas12->setVisible(editando);
    ui.btnMas24->setVisible(editando);
    ui.lblActivo->setVisible(editando);
    ui.swActivo->setVisible(editando);
    ui.lblActividad->setVisible(editando);
    ui.lstActividad->setVisible(editando);
    ui.btnCrear->setVisible(editando == false);

    if (editando == true) {
        Producto p = inventario->getProducto(posicionActual);
        QString estado = "Disponible en Vender";
        if (p.getActivo() == false) {
            estado = "No aparece en Vender";
        } else if (p.getExistencias() == 0) {
            estado = "Agotado";
        } else if (p.getExistencias() <= POCAS_EXISTENCIAS) {
            estado = "Quedan pocas";
        }
        ui.lblSubtitulo->setText(p.getCodigo() + "   ·   " + estado + "   ·   los cambios se guardan solos");
    }
}

// La lista de cambios del producto abierto, del más nuevo al más viejo
void ProductosForm::mostrarActividad()
{
    ui.lstActividad->clear();
    if (posicionActual == -1) {
        return;
    }
    QString hoy = QDate::currentDate().toString("dd/MM/yyyy");
    Producto p = inventario->getProducto(posicionActual);
    std::vector<Movimiento> lista = inventario->getMovimientos(p.getCodigo());
    for (int i = 0; i < lista.size() && i < 20; i++) {
        QString fecha = lista[i].getFecha();
        if (fecha.startsWith(hoy) == true) {
            fecha = "Hoy " + fecha.mid(11);
        }
        QListWidgetItem* item = new QListWidgetItem(lista[i].getTexto() + "\n" + fecha);
        ui.lstActividad->addItem(item);
    }
    if (lista.size() == 0) {
        ui.lstActividad->addItem("Todavía no hay cambios en este producto.");
    }
}

// Limpia la ficha para crear un producto, con el siguiente código ya puesto
void ProductosForm::nuevoProducto()
{
    posicionActual = -1;
    ui.tablaProductos->blockSignals(true);
    ui.tablaProductos->clearSelection();
    ui.tablaProductos->setCurrentCell(-1, -1);
    ui.tablaProductos->blockSignals(false);

    llenando = true;
    ui.txtCodigo->setText(siguienteCodigo());
    ui.txtNombre->clear();
    ui.spnPrecio->setValue(0);
    ui.spnExistencias->setValue(0);
    llenando = false;

    limpiarError();
    mostrarFicha();
    mostrarActividad();
    mostrarImagen(true);
    ui.txtNombre->setFocus();
}

void ProductosForm::crearProducto()
{
    if (posicionActual != -1) {
        return;
    }
    QString nombre = ui.txtNombre->text().trimmed();
    Producto nuevo(ui.txtCodigo->text().trimmed(), nombre, ui.spnPrecio->value(), ui.spnExistencias->value());
    QString error = inventario->agregar(nuevo);
    if (error != "") {
        mostrarError(error);
        return;
    }
    // Queda lista para crear otro, que es lo más común cuando llega mercadería nueva
    QString texto = "Se creó " + nombre + " (" + nuevo.getCodigo() + ")";
    if (guardarArchivo() == true) {
        nuevoProducto();
        actualizar();
        mostrarBarra(texto);
    }
}

// Guarda nombre, precio y existencias del producto abierto, si algo cambió
void ProductosForm::guardarDatos()
{
    if (posicionActual == -1 || llenando == true) {
        return;
    }
    int antes = inventario->getCantidadMovimientos();
    QString error = inventario->modificar(posicionActual, ui.txtNombre->text().trimmed(),
                                          ui.spnPrecio->value(), ui.spnExistencias->value());
    if (error != "") {
        mostrarError(error);
        return;
    }
    limpiarError();
    if (inventario->getCantidadMovimientos() != antes) {
        cambioHecho();
        // La tabla se refresca después, por si el cambio vino de hacer clic en otra fila
        QTimer::singleShot(0, this, &ProductosForm::actualizar);
    }
}

// Suma o resta existencias del producto abierto (botones − y +, y la mercadería que llega)
void ProductosForm::sumarExistencias(int cantidad, QString motivo)
{
    if (posicionActual == -1) {
        // En un producto nuevo solo cambia el número del campo
        ui.spnExistencias->setValue(ui.spnExistencias->value() + cantidad);
        return;
    }
    QString error = inventario->ajustarExistencias(posicionActual, cantidad, motivo);
    if (error != "") {
        mostrarError(error);
        return;
    }
    limpiarError();
    cargarCampos();
    cambioHecho();
    actualizar();
}

void ProductosForm::cambiarDisponible(bool disponible)
{
    if (posicionActual == -1 || llenando == true) {
        return;
    }
    if (inventario->getProducto(posicionActual).getActivo() != disponible) {
        inventario->cambiarEstado(posicionActual);
        cambioHecho();
        actualizar();
    }
}

void ProductosForm::deshacer()
{
    if (inventario->puedeDeshacer() == false) {
        mostrarBarra("No hay nada para deshacer");
        ui.btnDeshacer->hide();
        return;
    }
    QString cambio = inventario->getUltimoCambio();
    inventario->deshacer();
    guardarArchivo();
    if (posicionActual >= inventario->getCantidad()) {
        nuevoProducto();
    }
    cargarCampos();
    actualizar();
    mostrarBarra("Se deshizo: " + cambio);
}

// ---------------------------------------------------------------- Avisos

// Después de cada cambio: se guarda el archivo y aparece la barra para deshacer
void ProductosForm::cambioHecho()
{
    if (guardarArchivo() == true) {
        mostrarBarra("Guardado · " + inventario->getUltimoCambio());
    }
}

// Barra oscura abajo de la ficha, como la de "Deshacer" de Gmail o Fotos
void ProductosForm::mostrarBarra(QString texto)
{
    ui.lblDeshacer->setText(texto);
    ui.btnDeshacer->setVisible(inventario->puedeDeshacer());
    bool estabaOculta = ui.barraDeshacer->isHidden();
    ui.barraDeshacer->show();
    timerBarra.start();

    if (estabaOculta == true) {
        QGraphicsOpacityEffect* efecto = new QGraphicsOpacityEffect(ui.barraDeshacer);
        ui.barraDeshacer->setGraphicsEffect(efecto);
        QVariantAnimation* animacion = new QVariantAnimation(this);
        animacion->setDuration(240);
        animacion->setStartValue(0.0);
        animacion->setEndValue(1.0);
        animacion->setEasingCurve(QEasingCurve::OutCubic);
        QPointer<QGraphicsOpacityEffect> cuidado = efecto;
        connect(animacion, &QVariantAnimation::valueChanged, this, [cuidado](const QVariant& valor) {
            if (cuidado != nullptr) {
                cuidado->setOpacity(valor.toDouble());
            }
        });
        connect(animacion, &QVariantAnimation::finished, this, [this]() {
            ui.barraDeshacer->setGraphicsEffect(nullptr);
        });
        animacion->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

// El error aparece debajo de la ficha y el campo con el problema se marca en rojo
void ProductosForm::mostrarError(QString error)
{
    limpiarError();
    ui.lblError->setText(error);
    ui.lblError->show();
    QWidget* campo = nullptr;
    if (error.contains("código") == true) {
        campo = ui.txtCodigo;
    } else if (error.contains("nombre") == true) {
        campo = ui.txtNombre;
    } else if (error.contains("precio") == true) {
        campo = ui.spnPrecio;
    } else if (error.contains("existencias") == true) {
        campo = ui.spnExistencias;
    }
    if (campo != nullptr) {
        campo->setProperty("error", true);
        campo->style()->unpolish(campo);
        campo->style()->polish(campo);
        campo->setFocus();
    }
}

void ProductosForm::limpiarError()
{
    ui.lblError->clear();
    ui.lblError->hide();
    QWidget* campos[4] = {ui.txtCodigo, ui.txtNombre, ui.spnPrecio, ui.spnExistencias};
    for (int i = 0; i < 4; i++) {
        if (campos[i]->property("error").toBool() == true) {
            campos[i]->setProperty("error", false);
            campos[i]->style()->unpolish(campos[i]);
            campos[i]->style()->polish(campos[i]);
        }
    }
}

bool ProductosForm::guardarArchivo()
{
    if (ArchivoProductos::guardar(inventario) == false) {
        mostrarError("No se pudo guardar productos.txt");
        return false;
    }
    return true;
}

// ---------------------------------------------------------------- Imágenes

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

        // Solo cuentan las palabras de más de 2 letras, así "Azúcar 1 kg" da "AZ" y no "AK"
        QString iniciales;
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

// Cuadrito para la lista: el centro de la imagen, con esquinas redondeadas
QIcon ProductosForm::miniatura(QString codigo, QString nombre, bool activo) const
{
    QPixmap imagen = imagenProducto(codigo, nombre, activo);
    QPixmap centro = imagen.copy(230, 20, 300, 300).scaled(56, 56, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPixmap redondeada(56, 56);
    redondeada.fill(Qt::transparent);
    QPainter pintor(&redondeada);
    pintor.setRenderHint(QPainter::Antialiasing);
    pintor.setPen(Qt::NoPen);
    pintor.setBrush(QBrush(centro));
    pintor.drawRoundedRect(QRectF(0, 0, 56, 56), 12, 12);
    pintor.end();
    redondeada.setDevicePixelRatio(2.0);
    return QIcon(redondeada);
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
        if (animacionImagen == nullptr) {
            pintarImagen(1.0);
        }
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
