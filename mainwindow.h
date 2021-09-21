#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <sstream>
#include "ui_mainwindow.h"
#include "transaction_viewer.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include "transaction.h"
#include "bilan_viewer.h"
#include "CapitauxPropres.h"
#include "formulaire_date.h"
#include "rapprochement.h"
#include "all_transactions.h"
#include "fichierxml.h"
#include <QMouseEvent>
#include "all_rapprochements.h"


class bilan_viewer;
class Transaction_viewer;
class formulaire_date;
class capitaux_viewer;
class all_transactions;
class all_rapprochements;


/// \file mainwindow.h

/// \namespace Ui

namespace Ui {
class MainWindow;
}

/// \class MainWindow
/// \brief Fenêtre principale de l'application

/// Permet de naviguer dans l'application et de manipuler toutes les fonctionnalités du logiciel : ouvrir l'historique des transactions, le bilan...

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void choix_affichage(QListWidgetItem*);
    void on_Ajouter_Compte_clicked();
    void on_Supprimer_Compte_clicked();
    void writeXmlFile_comptes(const QString& filename);
    void writeXmlFile_transactions(const QString& filename);
    void MAJ_listeComptes();
    void reinitialiserXML();
    void on_Modifier_Compte_clicked();
public:
    explicit MainWindow(Transaction_viewer * v = nullptr, QWidget *parent = nullptr);
    ~MainWindow();
    void load_transactions(const QString& f);
    void addRoot(Compte_racine *c, QTreeWidgetItem * parent);
    void addEnfant(Compte_racine *c, QTreeWidgetItem * parent);
    /// \fn MainWindow::getUi
    /// \brief Renvoie l'attribut ui de notre mainwindow généré par Qt Designer
    Ui::MainWindow * getUi()const {return ui;}
    /// \fn MainWindow::mousePressEvent
    /// \brief permet d'annuler la sélection faite à la souris sur le QtreeWidget en cliquant à côté de l'arborescense affichée.
    void mousePressEvent(QMouseEvent *event)
    {
        QModelIndex item = ui->treeWidget->indexAt(event->pos());
        if (!item.isValid())
            ui->treeWidget->clearSelection();
        if(ui->treeWidget->currentItem()!=nullptr){
            ui->treeWidget->setCurrentItem(nullptr);
        }
    }
private slots:
    void on_Rapprocher_Compte_clicked();
    void on_actionOuvrir_triggered();

private:
    all_rapprochements *a;
    bilan_viewer *b;
    Transaction_viewer * v;
    formulaire_date * formulaire;
    capitaux_viewer * capitaux;
    all_transactions *transac;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
