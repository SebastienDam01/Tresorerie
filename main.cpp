#include <QApplication>
#include <QDebug>
#include <QFileDialog>
#include "mainwindow.h"
#include "transaction.h"
#include "transaction_viewer.h"
#include "compte.h"

/// \file main.cpp
/// \brief Construction de mainwindow et lancement de l'application
/// Avant d'afficher cette même application, nous chargeons les derniers fichiers XML utilisés
/// ou nous en créons des nouveaux si aucun fichier n'a été utilisé auparavant.

using namespace std;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // récupération des références sur les singletons Manager
    //création des comptes virtuels initiaux

    recup_last_fichier();
    CompteManager & cm = CompteManager::getManager();
    FichierXML & XML = FichierXML::getFichierXML();
    QString filename_compte = XML.getFichierCompte();
    QString filename_transaction = XML.getFichierTransaction();
    cm.load_comptes(filename_compte);
    transactionManager &tm=transactionManager::getManager();
    tm.load_transactions(filename_transaction);

    Transaction_viewer v;
    formulaire_date form;
    MainWindow fenetre(&v);
    v.setCreateur(&fenetre);

    fenetre.setWindowTitle("Logiciel de Tresorerie");
    fenetre.show();
    return app.exec();
}
