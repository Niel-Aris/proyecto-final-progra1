#include "Estilo.h"
#include <QFile>
#include <QStyleFactory>
#include <QPalette>
#include <QColor>
#include <QLabel>
#include <QLayout>
#include <QBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QTabBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QFontDatabase>
#include <QDialog>
#include <QEvent>
#include <QPointer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QEasingCurve>
#include <QGuiApplication>
#include <QStyleHints>
#include <QRegularExpression>
#include <QStringList>
#include <algorithm>
#include <map>
#include "Ajustes.h"

// Hace que los mensajes (QMessageBox) aparezcan suave en vez de golpe
class AnimadorDialogos : public QObject
{
public:
    AnimadorDialogos(QObject* parent) : QObject(parent) {}

protected:
    bool eventFilter(QObject* objeto, QEvent* evento) override
    {
        QDialog* dialogo = qobject_cast<QDialog*>(objeto);
        if (dialogo != nullptr && evento->type() == QEvent::Show && Ajustes::animaciones() == true) {
            dialogo->setWindowOpacity(0.0);
            QPropertyAnimation* animacion = new QPropertyAnimation(dialogo, "windowOpacity");
            animacion->setDuration(180);
            animacion->setStartValue(0.0);
            animacion->setEndValue(1.0);
            animacion->setEasingCurve(QEasingCurve::OutCubic);
            animacion->start(QAbstractAnimation::DeleteWhenStopped);
        }
        return false;
    }
};

// Colores de cada tema. La hoja estilo.qss usa los nombres con @ y aquí se cambian por el color.
// Primero va el claro y después el oscuro (parecidos a los de macOS).
static QString colorDe(QString nombre, bool oscuro)
{
    static std::map<QString, std::pair<QString, QString>> colores = {
        {"fondo", {"#f5f5f7", "#1c1c1e"}},
        {"tarjeta", {"#ffffff", "#2c2c2e"}},
        {"texto", {"#1d1d1f", "#f5f5f7"}},
        {"texto2", {"#424245", "#d1d1d6"}},
        {"gris", {"#6e6e73", "#98989d"}},
        {"gris2", {"#8e8e93", "#8e8e93"}},
        {"gris3", {"#a1a1a6", "#636366"}},
        {"borde", {"#d2d2d7", "#48484a"}},
        {"bordeSuave", {"#e0e0e5", "#3a3a3c"}},
        {"linea", {"#ececf0", "#38383a"}},
        {"alterno", {"#fafafc", "#303032"}},
        {"seleccion", {"#dcebfd", "#1d3d63"}},
        {"acento", {"#0071e3", "#0a84ff"}},
        {"acentoHover", {"#0077ed", "#409cff"}},
        {"acentoPresionado", {"#0062c4", "#0060df"}},
        {"acentoSuave", {"#e8f1fd", "#16324f"}},
        {"acentoSuave2", {"#d6e7fc", "#1d4170"}},
        {"acentoApagado", {"#b9d5f6", "#2a3f5a"}},
        {"neutro", {"#f2f2f5", "#3a3a3c"}},
        {"neutroHover", {"#e8e8ed", "#444446"}},
        {"neutroPresionado", {"#e6e6eb", "#4a4a4c"}},
        {"segmento", {"#e3e3e8", "#2c2c2e"}},
        {"segmentoHover", {"#d8d8de", "#3a3a3c"}},
        {"segmentoElegido", {"#ffffff", "#636366"}},
        {"scroll", {"#c7c7cc", "#5a5a5e"}},
        {"scrollHover", {"#aeaeb2", "#6e6e73"}},
        {"rojo", {"#e0352b", "#ff453a"}},
        {"rojoOscuro", {"#b8261e", "#ff6961"}},
        {"error", {"#d70015", "#ff453a"}},
        {"errorFondo", {"#fff5f5", "#3a1d1d"}},
        {"verde", {"#1a7f37", "#30d158"}},
        {"naranja", {"#c93400", "#ff9f0a"}},
        {"invertido", {"#1d1d1f", "#f5f5f7"}},
        {"sobreInvertido", {"#ffffff", "#1d1d1f"}},
        {"sobreAcento", {"#ffffff", "#ffffff"}},
        {"barra", {"#1d1d1f", "#3a3a3c"}},
        {"mesa", {"#e8e8ed", "#2a2a2c"}},
        {"interruptor", {"#e3e3e8", "#48484a"}},
    };
    if (colores.count(nombre) == 0) {
        return "#ff00ff";   // color feo a propósito, para notar si falta uno
    }
    if (oscuro == true) {
        return colores[nombre].second;
    }
    return colores[nombre].first;
}

static bool temaOscuro = false;
std::function<void()> Estilo::alCambiarTema;

void Estilo::cargar(QApplication* app)
{
    // Fusion se ve igual en Windows y en Mac, así la hoja de estilo no cambia
    app->setStyle(QStyleFactory::create("Fusion"));
    aplicarTema();
    app->installEventFilter(new AnimadorDialogos(app));

    // En "Automático" se sigue al sistema: si la Mac o Windows cambian a oscuro, la app también
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
    QObject::connect(QGuiApplication::styleHints(), &QStyleHints::colorSchemeChanged, app, []() {
        if (Ajustes::tema() == "auto") {
            Estilo::aplicarTema();
        }
    });
#endif
}

// Decide si va claro u oscuro, arma los colores y vuelve a cargar la hoja de estilo
void Estilo::aplicarTema()
{
    QApplication* app = qobject_cast<QApplication*>(QCoreApplication::instance());
    QString tema = Ajustes::tema();
    if (tema == "oscuro") {
        temaOscuro = true;
    } else if (tema == "claro") {
        temaOscuro = false;
    } else {
        temaOscuro = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
        temaOscuro = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;
#endif
    }

    QPalette colores;
    colores.setColor(QPalette::Window, color("fondo"));
    colores.setColor(QPalette::WindowText, color("texto"));
    colores.setColor(QPalette::Base, color("tarjeta"));
    colores.setColor(QPalette::AlternateBase, color("alterno"));
    colores.setColor(QPalette::Text, color("texto"));
    colores.setColor(QPalette::Button, color("tarjeta"));
    colores.setColor(QPalette::ButtonText, color("texto"));
    colores.setColor(QPalette::Highlight, color("acento"));
    colores.setColor(QPalette::HighlightedText, color("sobreAcento"));
    colores.setColor(QPalette::ToolTipBase, color("tarjeta"));
    colores.setColor(QPalette::ToolTipText, color("texto"));
    colores.setColor(QPalette::PlaceholderText, color("gris2"));
    colores.setColor(QPalette::Disabled, QPalette::Text, color("gris3"));
    colores.setColor(QPalette::Disabled, QPalette::ButtonText, color("gris3"));
    app->setPalette(colores);

    QFile archivo(":/estilo.qss");
    if (archivo.open(QIODevice::ReadOnly) == true) {
        QString hoja = QString::fromUtf8(archivo.readAll());
        archivo.close();
        // Se cambian los @nombre por su color. Los nombres largos van primero
        // para que @acento no se coma el comienzo de @acentoHover.
        QRegularExpression nombres("@([A-Za-z0-9]+)");
        QStringList encontrados;
        QRegularExpressionMatchIterator it = nombres.globalMatch(hoja);
        while (it.hasNext() == true) {
            QString nombre = it.next().captured(1);
            if (encontrados.contains(nombre) == false) {
                encontrados.append(nombre);
            }
        }
        std::sort(encontrados.begin(), encontrados.end(), [](const QString& a, const QString& b) { return a.size() > b.size(); });
        for (int i = 0; i < encontrados.size(); i++) {
            if (encontrados[i] != "nombre") {
                hoja.replace("@" + encontrados[i], colorDe(encontrados[i], temaOscuro));
            }
        }
        app->setStyleSheet(hoja);
    }

    // Las pantallas vuelven a pintar lo que dibujan con colores del código
    if (alCambiarTema) {
        alCambiarTema();
    }
}

QColor Estilo::color(QString nombre)
{
    return QColor(colorDe(nombre, temaOscuro));
}

bool Estilo::esOscuro()
{
    return temaOscuro;
}

void Estilo::aplicar(QMainWindow* ventana)
{
    ventana->setMinimumSize(900, 640);
    ventana->resize(1060, 720);

    // Márgenes de la ventana
    QBoxLayout* principal = qobject_cast<QBoxLayout*>(ventana->centralWidget()->layout());
    if (principal != nullptr) {
        principal->setContentsMargins(28, 20, 28, 24);
        principal->setSpacing(14);

    }

    // Pestañas: control segmentado y animación al cambiar
    QTabWidget* pestanas = ventana->findChild<QTabWidget*>();
    if (pestanas != nullptr) {
        pestanas->tabBar()->setDrawBase(false);
        pestanas->tabBar()->setExpanding(false);
        pestanas->tabBar()->setCursor(Qt::PointingHandCursor);
        for (int i = 0; i < pestanas->count(); i++) {
            QLayout* layoutPagina = pestanas->widget(i)->layout();
            if (layoutPagina != nullptr) {
                layoutPagina->setContentsMargins(0, 16, 0, 0);
                layoutPagina->setSpacing(18);
            }
            // En las pantallas de dos columnas, la tabla ocupa más que el panel
            QHBoxLayout* columnas = qobject_cast<QHBoxLayout*>(layoutPagina);
            if (columnas != nullptr && columnas->count() == 2) {
                columnas->setStretch(0, 3);
                columnas->setStretch(1, 2);
            }
        }
        QObject::connect(pestanas, &QTabWidget::currentChanged, pestanas, [pestanas](int indice) {
            Estilo::animarPagina(pestanas->widget(indice));
        });
    }

    // Tablas limpias, sin cuadrícula y con filas más altas
    QList<QTableWidget*> tablas = ventana->findChildren<QTableWidget*>();
    for (int i = 0; i < tablas.size(); i++) {
        tablas[i]->setShowGrid(false);
        tablas[i]->setAlternatingRowColors(true);
        tablas[i]->setSelectionBehavior(QAbstractItemView::SelectRows);
        tablas[i]->verticalHeader()->hide();
        tablas[i]->verticalHeader()->setDefaultSectionSize(36);
        tablas[i]->horizontalHeader()->setHighlightSections(false);
        tablas[i]->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        // Si la pantalla no acomoda sus columnas, se reparten a lo ancho
        if (tablas[i]->property("columnasPropias").toBool() == false) {
            tablas[i]->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        }
    }

    // Las etiquetas del formulario de productos centradas con su campo
    QList<QFormLayout*> formularios = ventana->findChildren<QFormLayout*>();
    for (int i = 0; i < formularios.size(); i++) {
        formularios[i]->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        formularios[i]->setVerticalSpacing(12);
        formularios[i]->setHorizontalSpacing(16);
    }

    // Los números con el mismo margen por dentro que los campos de texto
    QList<QSpinBox*> numeros = ventana->findChildren<QSpinBox*>();
    for (int i = 0; i < numeros.size(); i++) {
        QLineEdit* campo = numeros[i]->findChild<QLineEdit*>();
        if (campo != nullptr) {
            campo->setTextMargins(7, 0, 0, 0);
        }
    }

    QList<QPushButton*> botones = ventana->findChildren<QPushButton*>();
    for (int i = 0; i < botones.size(); i++) {
        botones[i]->setCursor(Qt::PointingHandCursor);
    }

    // El ticket con letra de recibo y que aparezca suave al cambiar
    QList<QPlainTextEdit*> textos = ventana->findChildren<QPlainTextEdit*>();
    for (int i = 0; i < textos.size(); i++) {
        QFont letra = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        letra.setPointSize(12);
        textos[i]->setFont(letra);
        QPlainTextEdit* texto = textos[i];
        QObject::connect(texto, &QPlainTextEdit::textChanged, texto, [texto]() {
            Estilo::aparecer(texto, 260, 0);
        });
    }
}

void Estilo::animarEntrada(QMainWindow* ventana)
{
    if (Ajustes::animaciones() == false) {
        return;
    }
    ventana->setWindowOpacity(0.0);
    QPropertyAnimation* animacion = new QPropertyAnimation(ventana, "windowOpacity");
    animacion->setDuration(420);
    animacion->setStartValue(0.0);
    animacion->setEndValue(1.0);
    animacion->setEasingCurve(QEasingCurve::OutCubic);
    animacion->start(QAbstractAnimation::DeleteWhenStopped);

    QTabWidget* pestanas = ventana->findChild<QTabWidget*>();
    if (pestanas != nullptr) {
        animarPagina(pestanas->currentWidget());
    }
}

void Estilo::animarPagina(QWidget* pagina)
{
    if (pagina != nullptr && Ajustes::animaciones() == true) {
        aparecer(pagina, 320, 14);
    }
}

// Sube la opacidad de 0 a 1 y, si hay desplazamiento, sube el widget unos píxeles
void Estilo::aparecer(QWidget* widget, int duracion, int desplazamiento)
{
    // Si ya había una animación en este widget, se detiene para no encimarlas
    QParallelAnimationGroup* anterior = widget->findChild<QParallelAnimationGroup*>("animacionEstilo", Qt::FindDirectChildrenOnly);
    if (anterior != nullptr) {
        anterior->stop();
        anterior->deleteLater();
    }

    QGraphicsOpacityEffect* efecto = new QGraphicsOpacityEffect(widget);
    efecto->setOpacity(0.0);
    widget->setGraphicsEffect(efecto);

    QParallelAnimationGroup* grupo = new QParallelAnimationGroup(widget);
    grupo->setObjectName("animacionEstilo");

    QPropertyAnimation* opacidad = new QPropertyAnimation(efecto, "opacity");
    opacidad->setDuration(duracion);
    opacidad->setStartValue(0.0);
    opacidad->setEndValue(1.0);
    opacidad->setEasingCurve(QEasingCurve::OutCubic);
    grupo->addAnimation(opacidad);

    if (desplazamiento != 0 && widget->parentWidget() != nullptr) {
        // La posición final es donde la pone la pestaña
        QPoint final = widget->parentWidget()->contentsRect().topLeft();
        QPropertyAnimation* movimiento = new QPropertyAnimation(widget, "pos");
        movimiento->setDuration(duracion + 80);
        movimiento->setStartValue(final + QPoint(0, desplazamiento));
        movimiento->setEndValue(final);
        movimiento->setEasingCurve(QEasingCurve::OutQuint);
        grupo->addAnimation(movimiento);
    }

    // Al terminar se quita el efecto para que el texto se vea nítido
    QObject::connect(grupo, &QParallelAnimationGroup::finished, widget, [widget, grupo]() {
        widget->setGraphicsEffect(nullptr);
        grupo->deleteLater();
    });
    grupo->start();
}

void Estilo::terminarEfectosArriba(QWidget* widget)
{
    QWidget* padre = widget->parentWidget();
    while (padre != nullptr) {
        if (padre->graphicsEffect() != nullptr) {
            padre->setGraphicsEffect(nullptr);
        }
        padre = padre->parentWidget();
    }
}
