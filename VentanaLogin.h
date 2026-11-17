#pragma once
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <vector>

// Inicio de sesión DE PRUEBA: no protege nada de verdad.
// Sirve para mostrar cómo se vería la app con usuarios. Los usuarios y el PIN están escritos aquí mismo.
class VentanaLogin : public QDialog
{
    Q_OBJECT

public:
    VentanaLogin(QWidget* parent = nullptr);
    QString getUsuario() const;

private:
    void elegirUsuario(int posicion);
    void revisarPin();
    void sacudir();

    std::vector<QString> usuarios;
    std::vector<QToolButton*> botones;
    QString usuarioElegido;
    QLineEdit* txtPin;
    QLabel* lblError;
    QPushButton* btnEntrar;
};
