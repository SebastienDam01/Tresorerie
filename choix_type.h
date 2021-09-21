#ifndef CHOIX_TYPE_H
#define CHOIX_TYPE_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "compte.h"
#include <QDialog>


/// \file choix_type.h
/// \class choix_type
/// \brief Widget qui permet de choisir le type d'un compte qui ne descend pas d'un compte virtuel


class choix_type : public QDialog{
    Q_OBJECT
    public:
    explicit choix_type(QDialog *parent = nullptr);
    void wait();
    signals:
    void WidgetClosed();
public slots:
    void annulation();
    type_compte choix();
    void closeEvent(QCloseEvent *event);
    private :
    bool selec = false;
    bool annul = false;
    QListWidget * type_dispo;
    QPushButton * selectionner;
    QPushButton * annuler;
    QHBoxLayout * layoutBouton;
    QVBoxLayout * layout;
};
#endif // CHOIX_TYPE_H
