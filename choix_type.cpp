#include "choix_type.h"
#include <unistd.h>
#include <QEvent>
#include <QCloseEvent>


/// \file choix_type.cpp
/// \brief Fichiers de fonctions pour la classe choix_type


/// \fn choix_type::choix_type
/// \brief Constructeur de la classe choix_type, un widget qui permet de sélectionner un type parmi les 4 déclarés quand on crée un compte descendant de la racine.
/// L'utilisateur pourra simplement sélectionner son type dans la liste qui lui sera affichée.
/// \param parent le widget parent

choix_type::choix_type(QDialog * parent) : QDialog (parent){
    setWindowTitle("Choix du type");
    type_dispo = new QListWidget();
    type_dispo->addItem(QString("Actif"));
    type_dispo->addItem(QString("Passif"));
    type_dispo->addItem(QString("Depense"));
    type_dispo->addItem(QString("Recette"));
    selectionner = new QPushButton("Selectionner");
    annuler = new QPushButton("Annuler");
    layoutBouton = new QHBoxLayout();
    layoutBouton->addWidget(selectionner);
    layoutBouton->addWidget(annuler);
    layout = new QVBoxLayout();
    layout->addWidget(type_dispo);
    layout->addLayout(layoutBouton);
    setLayout(layout);
    type_dispo->setCurrentRow(0);
    QObject::connect(selectionner,SIGNAL(clicked()),this,SLOT(choix()));
    QObject::connect(annuler,SIGNAL(clicked()),this,SLOT(annulation()));
}

/// \fn choix_type::closeEvent
/// \brief Cette fonction permet d'êmpecher l'utilisateur de fermer la fenêtre avec la croix, il doit donc cliquer sur le bouton annuler ou selectionner
/// \param event

void choix_type::closeEvent(QCloseEvent *event){
    event->ignore();
    if(selec == true)
        event->accept();
}

/// \fn choix_type::annulation
/// \brief Va avec la fonction closeEvent


void choix_type::annulation(){
    annul = true;
    choix();
}

/// \fn choix_type::choix
/// \brief Renvoie le type choisit par l'utilisateur
/// \return type_compte le type de compte choisit ou alors -1 si l'utilisateur clique sur cancel

type_compte choix_type::choix(){
    selec = true;
    close(); //On ferme, on ne detruit pas donc ça marche
    if(annul == true)
        return (type_compte) -1;
    return (type_compte)type_dispo->currentRow();
}
