#ifndef TRANSACTION_VIEWER_H
#define TRANSACTION_VIEWER_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QHeaderView>
#include <QDebug>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include "transaction.h"
#include "information.h"
#include "mainwindow.h"
class MainWindow;
class Information;

/// \file transaction_viewer.h
/// \class Transaction_viewer
/// \brief Classe Transaction_viewer : Widget permettant la consultation des transactions par rapport au compte sélectionné dans la QComboBox
///  mettant elle-même en lumière la liste de comptes classiques de l'assocation

class Transaction_viewer : public QWidget {
    Q_OBJECT
public:
    explicit Transaction_viewer(QWidget *parent = nullptr);
    void MAJ_liste_comptes();
    /// \fn Transaction_viewer::setCreateur
    /// \brief Permet de réaliser le lien entre la MainWindow et notre fenêtre Transaction_viewer.
    void setCreateur(MainWindow * f){this->mw = f;}
signals:
public slots:
    void refresh(int);
    void Ajout_transaction();
    void sup_transaction();
    void Modif_transaction();
private :
    MainWindow * mw;
    QLabel* comptel;
    QComboBox* listeComptes;
    QLabel* soldel;
    QLineEdit* solde;
    QHBoxLayout* compte_layout;
    QLabel* transfertsl;
    QTableWidget* vueTransactions;
    QVBoxLayout* couche;
    Information* info;
    QPushButton* bouton_modif;
};

#endif // TRANSACTION_VIEWER_H
