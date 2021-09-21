#ifndef CAPITAUXPROPRES_H
#define CAPITAUXPROPRES_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QInputDialog>
#include "transaction.h"
#include "mainwindow.h"

class MainWindow;


/// \file capitaux_viewer.h
/// \class capitaux_viewer
/// \brief Widget qui permet d'afficher, gérer et selectionner les comptes de capitaux propres faisant partie d'une transaction (solde initial)

class capitaux_viewer : public QWidget{
    Q_OBJECT
    public:
    explicit capitaux_viewer(Compte * c,QWidget *parent = nullptr);
    ///\fn capitaux_viewer::setCreateur
    /// \brief Permet de réaliser le lien entre la MainWindow et notre fenêtre capitaux_viewer.
    void setCreateur(MainWindow * f){this->mw = f;}
    signals:
    void WidgetClosed();
    public slots:
    void creer_compte_capitaux_propres();
    void new_transaction();
    private :
    Compte * new_compte;
    QLabel * capl;
    QListWidget * capitaux_propres;
    QPushButton * selectionner;
    QPushButton * ajouter;
    QVBoxLayout * layout;
    MainWindow * mw;
};

#endif // CAPITAUXPROPRES_H
