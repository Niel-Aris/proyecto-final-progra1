#include "HistorialForm.h"
#include "Imagenes.h"
#include "Estilo.h"
#include <QLocale>
#include <QDate>
#include <QPainter>
#include <QPen>
#include <QShortcut>
#include <QKeySequence>
#include <QClipboard>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollBar>
#include <map>

HistorialForm::HistorialForm(Historial* historial, QWidget* parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    this->historial = historial;
    ventaElegida = -1;
    ventasVistas = historial->getCantidad();

    // El recibo dibujado va dentro del área con scroll, por si la venta es larga
    vistaTicket = new VistaTicket();
    ui.scrTicket->setWidget(vistaTicket);
    ui.scrTicket->setFrameShape(QFrame::NoFrame);

    // Un botón de "Guardar" por cada formato de ticket (polimorfismo: todos son Ticket*)
    formatos.push_back(&ticketTxt);
    formatos.push_back(&ticketCsv);
    for (int i = 0; i < formatos.size(); i++) {
        QPushButton* boton = new QPushButton("Guardar ." + formatos[i]->getExtension());
        boton->setObjectName("btnFormato");
        boton->setCursor(Qt::PointingHandCursor);
        ui.layoutAcciones->insertWidget(i, boton);
        connect(boton, &QPushButton::clicked, this, [this, i]() { guardarTicket(i); });
    }
    ui.btnCopiar->setText("Copiar texto");
    ui.btnCopiar->setToolTip("Copia el ticket para pegarlo en WhatsApp o en un correo");
    connect(ui.btnCopiar, &QPushButton::clicked, this, &HistorialForm::copiarTicket);

    connect(ui.lstVentas, &QListWidget::currentItemChanged, this, &HistorialForm::elegirVenta);
    connect(ui.txtBuscar, &QLineEdit::textChanged, this, &HistorialForm::llenarLista);
    connect(ui.btnTodas, &QPushButton::clicked, this, &HistorialForm::llenarLista);
    connect(ui.btnHoy, &QPushButton::clicked, this, &HistorialForm::llenarLista);
    connect(ui.btnSemana, &QPushButton::clicked, this, &HistorialForm::llenarLista);

    QShortcut* buscar = new QShortcut(QKeySequence::Find, this);
    connect(buscar, &QShortcut::activated, this, [this]() {
        ui.txtBuscar->setFocus();
        ui.txtBuscar->selectAll();
    });
    QShortcut* copiar = new QShortcut(QKeySequence::Copy, ui.lstVentas, nullptr, nullptr, Qt::WidgetShortcut);
    connect(copiar, &QShortcut::activated, this, &HistorialForm::copiarTicket);
    ui.txtBuscar->setPlaceholderText("Buscar por número, fecha o producto   ("
                                     + QKeySequence(QKeySequence::Find).toString(QKeySequence::NativeText) + ")");

    ui.lblAviso->hide();
    timerAviso.setSingleShot(true);
    timerAviso.setInterval(4000);
    connect(&timerAviso, &QTimer::timeout, ui.lblAviso, &QWidget::hide);

    actualizar();
}

void HistorialForm::actualizar()
{
    // Si se cobró una venta nueva, se abre esa en vez de la que se estaba viendo
    if (historial->getCantidad() > ventasVistas) {
        ventaElegida = -1;
    }
    ventasVistas = historial->getCantidad();
    mostrarResumen();
    llenarLista();
}

QString HistorialForm::colones(int monto) const
{
    QLocale local(QLocale::Spanish, QLocale::CostaRica);
    return "₡" + local.toString(monto);
}

// "Hoy", "Ayer" o algo como "Lunes 15 de noviembre"
QString HistorialForm::diaDe(QString fecha) const
{
    QDate dia = QDate::fromString(fecha.left(10), "dd/MM/yyyy");
    if (dia.isValid() == false) {
        return fecha;
    }
    QDate hoy = QDate::currentDate();
    if (dia == hoy) {
        return "Hoy";
    }
    if (dia == hoy.addDays(-1)) {
        return "Ayer";
    }
    QLocale local(QLocale::Spanish, QLocale::CostaRica);
    QString texto = local.toString(dia, "dddd d 'de' MMMM");
    if (dia.year() != hoy.year()) {
        texto = texto + " de " + QString::number(dia.year());
    }
    texto[0] = texto[0].toUpper();
    return texto;
}

// Las ventas nuevas guardan la hora; las viejas solo la fecha
QString HistorialForm::horaDe(QString fecha) const
{
    if (fecha.size() > 10) {
        return fecha.mid(11);
    }
    return "";
}

// ---------------------------------------------------------------- Resumen

void HistorialForm::mostrarResumen()
{
    QDate hoy = QDate::currentDate();
    int totalHoy = 0;
    int ventasHoy = 0;
    int totalTodas = 0;
    std::map<QString, int> unidades;   // código → unidades vendidas
    std::map<QString, QString> nombres;

    for (int i = 0; i < historial->getCantidad(); i++) {
        Venta v = historial->getVenta(i);
        totalTodas = totalTodas + v.getTotal();
        if (QDate::fromString(v.getFecha().left(10), "dd/MM/yyyy") == hoy) {
            totalHoy = totalHoy + v.getTotal();
            ventasHoy++;
        }
        for (int j = 0; j < v.getCantidadLineas(); j++) {
            LineaVenta l = v.getLinea(j);
            unidades[l.getCodigo()] = unidades[l.getCodigo()] + l.getCantidad();
            nombres[l.getCodigo()] = l.getNombre();
        }
    }

    ui.lblHoyValor->setText(colones(totalHoy));
    if (ventasHoy == 1) {
        ui.lblHoyDetalle->setText("1 venta");
    } else {
        ui.lblHoyDetalle->setText(QString::number(ventasHoy) + " ventas");
    }

    if (historial->getCantidad() > 0) {
        ui.lblPromedioValor->setText(colones(totalTodas / historial->getCantidad()));
        ui.lblPromedioDetalle->setText("de " + QString::number(historial->getCantidad()) + " ventas");
    } else {
        ui.lblPromedioValor->setText(colones(0));
        ui.lblPromedioDetalle->setText("todavía no hay ventas");
    }

    QString mejor = "";
    int masUnidades = 0;
    for (std::map<QString, int>::iterator it = unidades.begin(); it != unidades.end(); it++) {
        if (it->second > masUnidades) {
            masUnidades = it->second;
            mejor = it->first;
        }
    }
    if (mejor == "") {
        ui.lblTopValor->setText("—");
        ui.lblTopDetalle->clear();
        ui.lblTopFoto->clear();
        ui.lblTopFoto->hide();
    } else {
        ui.lblTopValor->setText(nombres[mejor]);
        ui.lblTopDetalle->setText(QString::number(masUnidades) + " unidades vendidas");
        ui.lblTopFoto->setPixmap(Imagenes::cuadrada(mejor, nombres[mejor], true, 44));
        ui.lblTopFoto->show();
    }
}

// ---------------------------------------------------------------- Lista

// Revisa la búsqueda y el filtro de fecha
bool HistorialForm::pasaFiltro(Venta venta) const
{
    QDate dia = QDate::fromString(venta.getFecha().left(10), "dd/MM/yyyy");
    QDate hoy = QDate::currentDate();
    if (ui.btnHoy->isChecked() == true && dia != hoy) {
        return false;
    }
    if (ui.btnSemana->isChecked() == true && (dia.isValid() == false || dia < hoy.addDays(-6))) {
        return false;
    }

    QString texto = ui.txtBuscar->text().trimmed();
    if (texto.startsWith("#") == true) {
        texto = texto.mid(1);
    }
    if (texto == "") {
        return true;
    }
    if (QString::number(venta.getNumero()) == texto || venta.getFecha().contains(texto)) {
        return true;
    }
    for (int i = 0; i < venta.getCantidadLineas(); i++) {
        if (venta.getLinea(i).getNombre().contains(texto, Qt::CaseInsensitive) == true) {
            return true;
        }
    }
    return false;
}

// Las ventas de la más nueva a la más vieja, con un título por cada día
void HistorialForm::llenarLista()
{
    ui.lstVentas->blockSignals(true);
    ui.lstVentas->clear();

    QString diaAnterior = "";
    int mostradas = 0;
    int totalMostrado = 0;
    QListWidgetItem* elegida = nullptr;
    QListWidgetItem* primera = nullptr;

    for (int i = historial->getCantidad() - 1; i >= 0; i--) {
        Venta v = historial->getVenta(i);
        if (pasaFiltro(v) == false) {
            continue;
        }
        QString dia = diaDe(v.getFecha());
        if (dia != diaAnterior) {
            QListWidgetItem* titulo = new QListWidgetItem(ui.lstVentas);
            titulo->setFlags(Qt::NoItemFlags);
            titulo->setSizeHint(QSize(0, 34));
            QLabel* etiqueta = new QLabel(dia);
            etiqueta->setObjectName("encabezadoDia");
            ui.lstVentas->setItemWidget(titulo, etiqueta);
            diaAnterior = dia;
        }
        QListWidgetItem* item = new QListWidgetItem(ui.lstVentas);
        item->setData(Qt::UserRole, i);
        item->setSizeHint(QSize(0, 66));
        ui.lstVentas->setItemWidget(item, crearFila(v));
        if (primera == nullptr) {
            primera = item;
        }
        if (i == ventaElegida) {
            elegida = item;
        }
        mostradas++;
        totalMostrado = totalMostrado + v.getTotal();
    }
    ui.lstVentas->blockSignals(false);

    if (mostradas == 1) {
        ui.lblConteo->setText("1 venta   ·   " + colones(totalMostrado));
    } else {
        ui.lblConteo->setText(QString::number(mostradas) + " ventas   ·   " + colones(totalMostrado));
    }
    bool vacio = mostradas == 0;
    ui.lstVentas->setVisible(vacio == false);
    ui.lblVacio->setVisible(vacio);
    if (historial->getCantidad() > 0) {
        ui.lblVacio->setText("No hay ventas con esa búsqueda.\nPrueba con otro número, fecha o producto.");
    } else {
        ui.lblVacio->setText("Todavía no hay ventas.\nLas ventas que cobres en Vender aparecen aquí.");
    }

    // Se mantiene la venta que se estaba viendo; si no está, se abre la más reciente
    if (elegida == nullptr) {
        elegida = primera;
    }
    if (elegida != nullptr) {
        ui.lstVentas->setCurrentItem(elegida);
        ui.lstVentas->scrollToItem(elegida);
    } else {
        ventaElegida = -1;
        vistaTicket->limpiar();
    }
}

// Una venta en la lista: fotos de lo que llevó, número, hora, resumen y total
QWidget* HistorialForm::crearFila(Venta venta)
{
    QWidget* fila = new QWidget();
    fila->setObjectName("filaVenta");
    QHBoxLayout* horizontal = new QHBoxLayout(fila);
    horizontal->setContentsMargins(10, 6, 12, 6);
    horizontal->setSpacing(12);

    // Hasta tres fotos encimadas, como en las apps de pedidos
    int cuantas = qMin(3, venta.getCantidadLineas());
    int lado = 38;
    int corrimiento = 16;
    int ancho = lado + corrimiento * qMax(0, cuantas - 1);
    QPixmap fotos(ancho * 2, lado * 2);
    fotos.fill(Qt::transparent);
    fotos.setDevicePixelRatio(2.0);
    QPainter pintor(&fotos);
    pintor.setRenderHint(QPainter::Antialiasing);
    for (int i = cuantas - 1; i >= 0; i--) {
        LineaVenta l = venta.getLinea(i);
        double x = i * corrimiento;
        pintor.setPen(Qt::NoPen);
        pintor.setBrush(QColor("#ffffff"));
        pintor.drawRoundedRect(QRectF(x - 1, -1 + 1, lado + 2, lado), 10, 10);
        pintor.drawPixmap(QPointF(x, 1), Imagenes::cuadrada(l.getCodigo(), l.getNombre(), true, lado - 2));
    }
    pintor.end();
    QLabel* etiquetaFotos = new QLabel();
    etiquetaFotos->setPixmap(fotos);
    // Siempre del ancho de tres fotos, así los textos de todas las filas quedan alineados
    etiquetaFotos->setFixedSize(lado + corrimiento * 2, lado);
    etiquetaFotos->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    // Textos
    QVBoxLayout* textos = new QVBoxLayout();
    textos->setSpacing(1);
    QString arriba = "Venta #" + QString::number(venta.getNumero());
    QLabel* titulo = new QLabel(arriba);
    titulo->setObjectName("filaTitulo");
    QString resumen;
    int unidades = 0;
    for (int i = 0; i < venta.getCantidadLineas(); i++) {
        LineaVenta l = venta.getLinea(i);
        unidades = unidades + l.getCantidad();
        if (i > 0) {
            resumen = resumen + ", ";
        }
        resumen = resumen + l.getNombre();
        if (l.getCantidad() > 1) {
            resumen = resumen + " ×" + QString::number(l.getCantidad());
        }
    }
    QString hora = horaDe(venta.getFecha());
    QString detalle = QString::number(unidades) + (unidades == 1 ? " artículo" : " artículos");
    if (hora != "") {
        detalle = hora + "   ·   " + detalle;
    }
    QLabel* etiquetaDetalle = new QLabel(detalle);
    etiquetaDetalle->setObjectName("filaDetalle");
    QLabel* etiquetaResumen = new QLabel();
    etiquetaResumen->setObjectName("filaResumen");
    etiquetaResumen->setText(etiquetaResumen->fontMetrics().elidedText(resumen, Qt::ElideRight, 230));
    etiquetaResumen->setToolTip(resumen);
    textos->addWidget(titulo);
    textos->addWidget(etiquetaDetalle);
    textos->addWidget(etiquetaResumen);

    QLabel* total = new QLabel(colones(venta.getTotal()));
    total->setObjectName("filaTotal");
    total->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    horizontal->addWidget(etiquetaFotos);
    horizontal->addLayout(textos, 1);
    horizontal->addWidget(total);
    return fila;
}

void HistorialForm::elegirVenta(QListWidgetItem* actual)
{
    if (actual == nullptr || (actual->flags() & Qt::ItemIsSelectable) == 0) {
        return;   // es un título de día, no una venta
    }
    int posicion = actual->data(Qt::UserRole).toInt();
    if (posicion < 0 || posicion >= historial->getCantidad()) {
        return;
    }
    bool esOtra = posicion != ventaElegida;
    ventaElegida = posicion;
    vistaTicket->mostrarVenta(historial->getVenta(posicion), esOtra);
    if (esOtra == true) {
        ui.scrTicket->verticalScrollBar()->setValue(0);
    }
}

// ---------------------------------------------------------------- Acciones

// El mismo código sirve para cualquier formato (polimorfismo)
void HistorialForm::guardarTicket(int formato)
{
    if (ventaElegida == -1) {
        mostrarAviso("Elige una venta primero.", true);
        return;
    }
    Venta venta = historial->getVenta(ventaElegida);
    Ticket* ticket = formatos[formato];
    QString archivo = "ticket-" + QString::number(venta.getNumero()) + "." + ticket->getExtension();
    if (ticket->guardar(venta) == true) {
        mostrarAviso("Se guardó " + archivo + " en la carpeta del programa.", false);
    } else {
        mostrarAviso("No se pudo guardar " + archivo + ".", true);
    }
}

void HistorialForm::copiarTicket()
{
    if (ventaElegida == -1) {
        mostrarAviso("Elige una venta primero.", true);
        return;
    }
    QGuiApplication::clipboard()->setText(ticketTxt.generar(historial->getVenta(ventaElegida)));
    mostrarAviso("Ticket copiado. Se puede pegar en WhatsApp o en un correo.", false);
}

void HistorialForm::mostrarAviso(QString texto, bool esError)
{
    if (esError == true) {
        ui.lblAviso->setStyleSheet("color: " + Estilo::color("error").name() + ";");
    } else {
        ui.lblAviso->setStyleSheet("color: " + Estilo::color("verde").name() + ";");
    }
    ui.lblAviso->setText(texto);
    ui.lblAviso->show();
    timerAviso.start();
}
