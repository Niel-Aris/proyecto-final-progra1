#include "MainWindow.h"
#include "ArchivoProductos.h"
#include "ArchivoVentas.h"
#include "Ajustes.h"
#include "Estilo.h"
#include "Iconos.h"
#include "VentanaAjustes.h"
#include "VentanaLogin.h"
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QShortcut>
#include <QKeySequence>
#include <QLocale>
#include <QDate>
#include <QPixmap>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // Leer los datos guardados
    if (ArchivoProductos::cargar(&inventario) == false || ArchivoVentas::cargar(&historial) == false) {
        QMessageBox::warning(this, "Error", "No se pudieron leer los archivos de datos.");
    }

    // Crear las tres pantallas y ponerlas en pestañas
    productosForm = new ProductosForm(&inventario);
    ventasForm = new VentasForm(&inventario, &historial);
    historialForm = new HistorialForm(&historial);

    ui.tabWidget->addTab(productosForm, "Productos");
    ui.tabWidget->addTab(ventasForm, "Vender");
    ui.tabWidget->addTab(historialForm, "Historial");

    connect(ui.tabWidget, &QTabWidget::currentChanged, this, &MainWindow::cambiarPestana);

    crearEncabezado();

    // Atajos: ⌘1, ⌘2 y ⌘3 cambian de pestaña y ⌘, abre los ajustes (como en las apps de Mac)
    for (int i = 0; i < 3; i++) {
        QShortcut* atajo = new QShortcut(QKeySequence(Qt::CTRL | (Qt::Key_1 + i)), this);
        connect(atajo, &QShortcut::activated, this, [this, i]() { ui.tabWidget->setCurrentIndex(i); });
        ui.tabWidget->setTabToolTip(i, QKeySequence(Qt::CTRL | (Qt::Key_1 + i)).toString(QKeySequence::NativeText));
    }
    QShortcut* ajustes = new QShortcut(QKeySequence::Preferences, this);
    connect(ajustes, &QShortcut::activated, this, &MainWindow::abrirAjustes);
    QShortcut* ajustesCtrl = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_Comma), this);
    connect(ajustesCtrl, &QShortcut::activated, this, &MainWindow::abrirAjustes);

    // Cuando cambia el tema (por el botón, los ajustes o el sistema) se vuelve a pintar todo
    Estilo::alCambiarTema = [this]() { refrescarTodo(); };

    setUsuario(Ajustes::ultimoUsuario());
}

MainWindow::~MainWindow()
{
    Estilo::alCambiarTema = nullptr;   // la ventana ya no existe, no hay nada que repintar
}

// Logo, nombre de la pulpería y fecha a la izquierda; tema, ajustes y usuario a la derecha
void MainWindow::crearEncabezado()
{
    QWidget* encabezado = new QWidget();
    encabezado->setObjectName("encabezado");
    QHBoxLayout* fila = new QHBoxLayout(encabezado);
    fila->setContentsMargins(0, 0, 0, 0);
    fila->setSpacing(12);

    QLabel* logo = new QLabel();
    logo->setPixmap(QPixmap(":/imagenes/logo-chico.png"));
    logo->setScaledContents(true);
    logo->setFixedSize(44, 44);

    QVBoxLayout* textos = new QVBoxLayout();
    textos->setSpacing(0);
    lblMarca = new QLabel();
    lblMarca->setObjectName("lblMarca");
    lblFecha = new QLabel();
    lblFecha->setObjectName("lblLema");
    textos->addWidget(lblMarca);
    textos->addWidget(lblFecha);

    btnTema = new QPushButton();
    btnTema->setObjectName("btnTema");
    btnTema->setIconSize(QSize(18, 18));
    btnTema->setCursor(Qt::PointingHandCursor);
    connect(btnTema, &QPushButton::clicked, this, &MainWindow::cambiarTema);

    btnAjustes = new QPushButton();
    btnAjustes->setObjectName("btnAjustes");
    btnAjustes->setIconSize(QSize(18, 18));
    btnAjustes->setCursor(Qt::PointingHandCursor);
    btnAjustes->setToolTip("Ajustes (" + QKeySequence(QKeySequence::Preferences).toString(QKeySequence::NativeText) + ")");
    connect(btnAjustes, &QPushButton::clicked, this, &MainWindow::abrirAjustes);

    // Botón del usuario con su menú
    btnUsuario = new QPushButton();
    btnUsuario->setObjectName("btnUsuario");
    btnUsuario->setIconSize(QSize(26, 26));
    btnUsuario->setCursor(Qt::PointingHandCursor);
    QMenu* menu = new QMenu(btnUsuario);
    menu->addAction("Ajustes…", this, &MainWindow::abrirAjustes);
    menu->addSeparator();
    menu->addAction("Cerrar sesión", this, &MainWindow::cerrarSesion);
    btnUsuario->setMenu(menu);

    fila->addWidget(logo);
    fila->addLayout(textos);
    fila->addStretch();
    fila->addWidget(btnTema);
    fila->addWidget(btnAjustes);
    fila->addWidget(btnUsuario);
    ui.layoutCentral->insertWidget(0, encabezado);

    refrescarTodo();
}

void MainWindow::setUsuario(QString usuario)
{
    this->usuario = usuario;
    btnUsuario->setText(usuario);
    btnUsuario->setIcon(QIcon(Iconos::avatar(usuario, 26)));
    refrescarTodo();
}

// Cada vez que se cambia de pestaña, las pantallas vuelven a leer los datos
void MainWindow::cambiarPestana(int indice)
{
    productosForm->actualizar();
    ventasForm->actualizar();
    historialForm->actualizar();
}

// Vuelve a escribir el encabezado y a llenar las pantallas (por ejemplo, después de cambiar el tema)
void MainWindow::refrescarTodo()
{
    QLocale local(QLocale::Spanish, QLocale::CostaRica);
    QString fecha = local.toString(QDate::currentDate(), "dddd d 'de' MMMM");
    fecha[0] = fecha[0].toUpper();
    lblMarca->setText(Ajustes::nombreTienda());
    if (usuario == "") {
        lblFecha->setText(fecha);
    } else {
        lblFecha->setText(fecha + "   ·   Hola, " + usuario);
    }
    setWindowTitle(Ajustes::nombreTienda());

    QColor colorIcono = Estilo::color("texto2");
    if (Estilo::esOscuro() == true) {
        btnTema->setIcon(Iconos::sol(colorIcono));
        btnTema->setToolTip("Cambiar a modo claro");
    } else {
        btnTema->setIcon(Iconos::luna(colorIcono));
        btnTema->setToolTip("Cambiar a modo oscuro");
    }
    btnAjustes->setIcon(Iconos::engrane(colorIcono));

    cambiarPestana(ui.tabWidget->currentIndex());
}

// El botón de la luna o el sol cambia entre claro y oscuro
void MainWindow::cambiarTema()
{
    if (Estilo::esOscuro() == true) {
        Ajustes::setTema("claro");
    } else {
        Ajustes::setTema("oscuro");
    }
    Estilo::aplicarTema();
}

void MainWindow::abrirAjustes()
{
    VentanaAjustes ventana(this);
    ventana.exec();
    refrescarTodo();
}

// Se esconde la ventana y vuelve el inicio de sesión. Si lo cierran, se sale del programa.
void MainWindow::cerrarSesion()
{
    hide();
    VentanaLogin login;
    if (login.exec() == QDialog::Accepted) {
        setUsuario(login.getUsuario());
        show();
        Estilo::animarEntrada(this);
    } else {
        close();
    }
}
