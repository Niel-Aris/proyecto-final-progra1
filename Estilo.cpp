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

// Hace que los mensajes (QMessageBox) aparezcan suave en vez de golpe
class AnimadorDialogos : public QObject
{
public:
    AnimadorDialogos(QObject* parent) : QObject(parent) {}

protected:
    bool eventFilter(QObject* objeto, QEvent* evento) override
    {
        QDialog* dialogo = qobject_cast<QDialog*>(objeto);
        if (dialogo != nullptr && evento->type() == QEvent::Show) {
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

void Estilo::cargar(QApplication* app)
{
    // Fusion se ve igual en Windows y en Mac, así la hoja de estilo no cambia
    app->setStyle(QStyleFactory::create("Fusion"));

    // Colores claros aunque el sistema esté en modo oscuro
    QPalette colores;
    colores.setColor(QPalette::Window, QColor("#f5f5f7"));
    colores.setColor(QPalette::WindowText, QColor("#1d1d1f"));
    colores.setColor(QPalette::Base, QColor("#ffffff"));
    colores.setColor(QPalette::AlternateBase, QColor("#fafafc"));
    colores.setColor(QPalette::Text, QColor("#1d1d1f"));
    colores.setColor(QPalette::Button, QColor("#ffffff"));
    colores.setColor(QPalette::ButtonText, QColor("#1d1d1f"));
    colores.setColor(QPalette::Highlight, QColor("#0071e3"));
    colores.setColor(QPalette::HighlightedText, QColor("#ffffff"));
    colores.setColor(QPalette::ToolTipBase, QColor("#ffffff"));
    colores.setColor(QPalette::ToolTipText, QColor("#1d1d1f"));
    colores.setColor(QPalette::PlaceholderText, QColor("#8e8e93"));
    colores.setColor(QPalette::Disabled, QPalette::Text, QColor("#a1a1a6"));
    colores.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#a1a1a6"));
    app->setPalette(colores);

    QFile archivo(":/estilo.qss");
    if (archivo.open(QIODevice::ReadOnly) == true) {
        app->setStyleSheet(QString::fromUtf8(archivo.readAll()));
        archivo.close();
    }

    app->installEventFilter(new AnimadorDialogos(app));
}

void Estilo::aplicar(QMainWindow* ventana)
{
    ventana->setWindowTitle("La Pulpe");
    ventana->setMinimumSize(900, 640);
    ventana->resize(1060, 720);

    // Márgenes de la ventana
    QBoxLayout* principal = qobject_cast<QBoxLayout*>(ventana->centralWidget()->layout());
    if (principal != nullptr) {
        principal->setContentsMargins(28, 20, 28, 24);
        principal->setSpacing(14);

        // Encabezado con el nombre de la pulpería
        QWidget* encabezado = new QWidget();
        QHBoxLayout* filaEncabezado = new QHBoxLayout(encabezado);
        filaEncabezado->setContentsMargins(4, 0, 4, 0);
        filaEncabezado->setSpacing(12);
        QLabel* marca = new QLabel("La Pulpe");
        marca->setObjectName("lblMarca");
        QLabel* lema = new QLabel("Inventario, ventas y tickets");
        lema->setObjectName("lblLema");
        filaEncabezado->addWidget(marca, 0, Qt::AlignBottom);
        filaEncabezado->addWidget(lema, 0, Qt::AlignBottom);
        filaEncabezado->addStretch();
        principal->insertWidget(0, encabezado);
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
    if (pagina != nullptr) {
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
