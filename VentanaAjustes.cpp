#include "VentanaAjustes.h"
#include "Ajustes.h"
#include "Estilo.h"
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QHBoxLayout>
#include <QPixmap>
#include <QScrollArea>

VentanaAjustes::VentanaAjustes(QWidget* parent)
    : QDialog(parent)
{
    setObjectName("VentanaAjustes");
    setWindowTitle("Ajustes");
    setMinimumSize(520, 640);

    QVBoxLayout* exterior = new QVBoxLayout(this);
    exterior->setContentsMargins(0, 0, 0, 0);
    QScrollArea* desplazar = new QScrollArea();
    desplazar->setObjectName("scrAjustes");
    desplazar->setWidgetResizable(true);
    desplazar->setFrameShape(QFrame::NoFrame);
    QWidget* contenido = new QWidget();
    contenido->setObjectName("contenidoAjustes");
    desplazar->setWidget(contenido);
    exterior->addWidget(desplazar);

    QVBoxLayout* columna = new QVBoxLayout(contenido);
    columna->setContentsMargins(28, 24, 28, 24);
    columna->setSpacing(10);
    QLabel* titulo = new QLabel("Ajustes");
    titulo->setObjectName("tituloAjustes");
    columna->addWidget(titulo);

    // ---- Apariencia
    QFrame* apariencia = crearTarjeta("Apariencia", columna);
    QWidget* segmento = new QWidget();
    QHBoxLayout* filaTema = new QHBoxLayout(segmento);
    filaTema->setContentsMargins(0, 0, 0, 0);
    filaTema->setSpacing(4);
    btnAuto = new QPushButton("Automático");
    btnClaro = new QPushButton("Claro");
    btnOscuro = new QPushButton("Oscuro");
    QPushButton* temas[3] = {btnAuto, btnClaro, btnOscuro};
    QString nombres[3] = {"auto", "claro", "oscuro"};
    for (int i = 0; i < 3; i++) {
        temas[i]->setObjectName("segmentoAjuste");
        temas[i]->setCheckable(true);
        temas[i]->setAutoExclusive(true);
        temas[i]->setCursor(Qt::PointingHandCursor);
        temas[i]->setChecked(Ajustes::tema() == nombres[i]);
        QString nombre = nombres[i];
        connect(temas[i], &QPushButton::clicked, this, [this, nombre]() { elegirTema(nombre); });
        filaTema->addWidget(temas[i]);
    }
    agregarFila(apariencia, "Tema", "Automático sigue al modo claro u oscuro de la computadora", segmento);

    Interruptor* animaciones = new Interruptor();
    animaciones->setChecked(Ajustes::animaciones());
    connect(animaciones, &QAbstractButton::toggled, this, [](bool activas) { Ajustes::setAnimaciones(activas); });
    agregarFila(apariencia, "Animaciones", "Transiciones, el recibo que se imprime y la venta lista", animaciones);

    // ---- Pulpería
    QFrame* pulperia = crearTarjeta("Pulpería", columna);
    QLineEdit* nombre = new QLineEdit(Ajustes::nombreTienda());
    nombre->setObjectName("txtNombreTienda");
    nombre->setFixedWidth(200);
    nombre->setMaxLength(30);
    connect(nombre, &QLineEdit::editingFinished, this, [nombre]() { Ajustes::setNombreTienda(nombre->text()); });
    agregarFila(pulperia, "Nombre", "Sale arriba en la ventana y en los tickets", nombre);

    QSpinBox* pocas = new QSpinBox();
    pocas->setRange(1, 100);
    pocas->setValue(Ajustes::pocasExistencias());
    pocas->setSuffix(" o menos");
    pocas->setFixedWidth(130);
    connect(pocas, &QSpinBox::valueChanged, this, [](int cantidad) { Ajustes::setPocasExistencias(cantidad); });
    agregarFila(pulperia, "Avisar \"por agotarse\"", "Cuando a un producto le queden estas unidades", pocas);

    // ---- Sesión
    QFrame* sesion = crearTarjeta("Sesión", columna);
    Interruptor* pedirPin = new Interruptor();
    pedirPin->setChecked(Ajustes::pedirPin());
    connect(pedirPin, &QAbstractButton::toggled, this, [](bool pedir) { Ajustes::setPedirPin(pedir); });
    agregarFila(sesion, "Pedir PIN al abrir", "Es un inicio de sesión de prueba (PIN 1234), no protege los datos", pedirPin);

    // ---- Acerca de
    QFrame* acerca = crearTarjeta("Acerca de", columna);
    QHBoxLayout* filaAcerca = new QHBoxLayout();
    filaAcerca->setSpacing(14);
    QLabel* logo = new QLabel();
    logo->setPixmap(QPixmap(":/imagenes/logo.png"));
    logo->setScaledContents(true);
    logo->setFixedSize(56, 56);
    QLabel* texto = new QLabel("<b>La Pulpe 1.0</b><br>"
                               "Proyecto final de Programación I (EIF201)<br>"
                               "Universidad Nacional, Campus Nicoya · II ciclo 2026<br>"
                               "Hecho por Carlos, Nahomy y Maccoy");
    texto->setObjectName("textoAcerca");
    filaAcerca->addWidget(logo, 0, Qt::AlignTop);
    filaAcerca->addWidget(texto, 1);
    qobject_cast<QVBoxLayout*>(acerca->layout())->addLayout(filaAcerca);

    columna->addStretch();
    QPushButton* listo = new QPushButton("Listo");
    listo->setObjectName("btnListo");
    listo->setCursor(Qt::PointingHandCursor);
    listo->setDefault(true);
    connect(listo, &QPushButton::clicked, this, [this, nombre]() {
        Ajustes::setNombreTienda(nombre->text());
        accept();
    });
    columna->addWidget(listo, 0, Qt::AlignRight);
}

// Título de sección y una tarjeta blanca debajo, como en Ajustes del iPhone
QFrame* VentanaAjustes::crearTarjeta(QString titulo, QVBoxLayout* columna)
{
    QLabel* etiqueta = new QLabel(titulo);
    etiqueta->setObjectName("seccionAjustes");
    columna->addSpacing(8);
    columna->addWidget(etiqueta);
    QFrame* tarjeta = new QFrame();
    tarjeta->setObjectName("tarjetaAjustes");
    QVBoxLayout* filas = new QVBoxLayout(tarjeta);
    filas->setContentsMargins(16, 10, 16, 10);
    filas->setSpacing(10);
    columna->addWidget(tarjeta);
    return tarjeta;
}

void VentanaAjustes::agregarFila(QFrame* tarjeta, QString texto, QString detalle, QWidget* control)
{
    QHBoxLayout* fila = new QHBoxLayout();
    fila->setSpacing(16);
    QVBoxLayout* textos = new QVBoxLayout();
    textos->setSpacing(1);
    QLabel* principal = new QLabel(texto);
    principal->setObjectName("filaAjusteTexto");
    QLabel* explicacion = new QLabel(detalle);
    explicacion->setObjectName("filaAjusteDetalle");
    explicacion->setWordWrap(true);
    textos->addWidget(principal);
    textos->addWidget(explicacion);
    fila->addLayout(textos, 1);
    fila->addWidget(control, 0, Qt::AlignVCenter);
    qobject_cast<QVBoxLayout*>(tarjeta->layout())->addLayout(fila);
}

// El tema cambia en el momento, para ver cómo queda
void VentanaAjustes::elegirTema(QString tema)
{
    Ajustes::setTema(tema);
    Estilo::aplicarTema();
}
