#include "VentanaLogin.h"
#include "Ajustes.h"
#include "Iconos.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QRegularExpressionValidator>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QFont>

// El PIN de prueba de todos los usuarios
const QString PIN_DE_PRUEBA = "1234";

VentanaLogin::VentanaLogin(QWidget* parent)
    : QDialog(parent)
{
    setObjectName("VentanaLogin");
    setWindowTitle("Iniciar sesión");
    setFixedSize(440, 560);

    usuarios.push_back("Administrador");
    usuarios.push_back("Cajero");

    QVBoxLayout* columna = new QVBoxLayout(this);
    columna->setContentsMargins(40, 36, 40, 32);
    columna->setSpacing(10);

    // Logo y bienvenida
    QLabel* logo = new QLabel();
    logo->setPixmap(QPixmap(":/imagenes/logo.png"));
    logo->setFixedSize(96, 96);
    logo->setScaledContents(true);
    QLabel* bienvenida = new QLabel(Ajustes::nombreTienda());
    bienvenida->setObjectName("lblBienvenida");
    QLabel* subtitulo = new QLabel("Bienvenido. Elige tu usuario e ingresa tu PIN");
    subtitulo->setObjectName("lblSubtituloLogin");
    columna->addWidget(logo, 0, Qt::AlignHCenter);
    columna->addSpacing(6);
    columna->addWidget(bienvenida, 0, Qt::AlignHCenter);
    columna->addWidget(subtitulo, 0, Qt::AlignHCenter);
    columna->addSpacing(14);

    // Un botón por usuario, con su avatar
    QHBoxLayout* filaUsuarios = new QHBoxLayout();
    filaUsuarios->setSpacing(14);
    filaUsuarios->addStretch();
    for (int i = 0; i < usuarios.size(); i++) {
        QToolButton* boton = new QToolButton();
        boton->setObjectName("btnUsuarioLogin");
        boton->setText(usuarios[i]);
        boton->setIcon(QIcon(Iconos::avatar(usuarios[i], 56)));
        boton->setIconSize(QSize(56, 56));
        boton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        boton->setCheckable(true);
        boton->setAutoExclusive(true);
        boton->setCursor(Qt::PointingHandCursor);
        boton->setFixedSize(124, 112);
        connect(boton, &QToolButton::clicked, this, [this, i]() { elegirUsuario(i); });
        botones.push_back(boton);
        filaUsuarios->addWidget(boton);
    }
    filaUsuarios->addStretch();
    columna->addLayout(filaUsuarios);
    columna->addSpacing(14);

    // PIN: 4 números. Al escribir el cuarto entra solo, como en el iPhone.
    txtPin = new QLineEdit();
    txtPin->setObjectName("txtPin");
    txtPin->setEchoMode(QLineEdit::Password);
    txtPin->setMaxLength(4);
    txtPin->setAlignment(Qt::AlignCenter);
    txtPin->setPlaceholderText("••••");
    txtPin->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]{0,4}"), txtPin));
    QFont letra = txtPin->font();
    letra.setPixelSize(22);
    letra.setLetterSpacing(QFont::AbsoluteSpacing, 10);
    txtPin->setFont(letra);
    txtPin->setFixedWidth(220);
    connect(txtPin, &QLineEdit::textChanged, this, [this](const QString& texto) {
        // El aviso de error se borra cuando la persona vuelve a escribir, no cuando se limpia el campo
        if (texto != "") {
            lblError->setText(" ");
        }
        if (texto.size() == 4) {
            revisarPin();
        }
    });
    connect(txtPin, &QLineEdit::returnPressed, this, &VentanaLogin::revisarPin);
    columna->addWidget(txtPin, 0, Qt::AlignHCenter);

    lblError = new QLabel(" ");
    lblError->setObjectName("lblErrorLogin");
    columna->addWidget(lblError, 0, Qt::AlignHCenter);

    btnEntrar = new QPushButton("Entrar");
    btnEntrar->setObjectName("btnEntrar");
    btnEntrar->setCursor(Qt::PointingHandCursor);
    btnEntrar->setFixedWidth(220);
    connect(btnEntrar, &QPushButton::clicked, this, &VentanaLogin::revisarPin);
    columna->addWidget(btnEntrar, 0, Qt::AlignHCenter);

    columna->addStretch();
    QLabel* pista = new QLabel("Inicio de sesión de prueba · PIN: " + PIN_DE_PRUEBA);
    pista->setObjectName("lblPista");
    columna->addWidget(pista, 0, Qt::AlignHCenter);

    // Queda elegido el último usuario que entró
    int ultimo = 0;
    for (int i = 0; i < usuarios.size(); i++) {
        if (usuarios[i] == Ajustes::ultimoUsuario()) {
            ultimo = i;
        }
    }
    botones[ultimo]->setChecked(true);
    elegirUsuario(ultimo);
}

QString VentanaLogin::getUsuario() const
{
    return usuarioElegido;
}

void VentanaLogin::elegirUsuario(int posicion)
{
    usuarioElegido = usuarios[posicion];
    txtPin->clear();
    txtPin->setFocus();
}

void VentanaLogin::revisarPin()
{
    if (txtPin->text() == PIN_DE_PRUEBA) {
        Ajustes::setUltimoUsuario(usuarioElegido);
        accept();
        return;
    }
    lblError->setText("PIN incorrecto. Intenta de nuevo.");
    txtPin->clear();
    sacudir();
}

// El campo del PIN se mueve de lado a lado, como cuando el iPhone no reconoce el código
void VentanaLogin::sacudir()
{
    if (Ajustes::animaciones() == false) {
        return;
    }
    QPoint inicio = txtPin->pos();
    QPropertyAnimation* animacion = new QPropertyAnimation(txtPin, "pos", this);
    animacion->setDuration(420);
    animacion->setKeyValueAt(0.0, inicio);
    animacion->setKeyValueAt(0.15, inicio + QPoint(-12, 0));
    animacion->setKeyValueAt(0.35, inicio + QPoint(10, 0));
    animacion->setKeyValueAt(0.55, inicio + QPoint(-7, 0));
    animacion->setKeyValueAt(0.75, inicio + QPoint(4, 0));
    animacion->setKeyValueAt(1.0, inicio);
    animacion->start(QAbstractAnimation::DeleteWhenStopped);
}
