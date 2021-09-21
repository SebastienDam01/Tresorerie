#include "formulaire_date.h"


/// \file formulaire_date.cpp
/// \brief Fichiers de fonctions pour la classe formulaire_date


/// \fn formulaire_date::formulaire_date
/// \brief Constructeur du formulaire de dates pour la génération du compte de résultat
/// \param parent le widget parent

formulaire_date::formulaire_date(QWidget *parent): QWidget(parent){

    date_debut = new QLabel("Veuillez rentrer la date de début");
    date_fin= new QLabel("Veuillez rentrer la date de fin");

    date_ecrite1 = new QDateEdit;
    date_ecrite2= new QDateEdit;

    date_layout1= new QHBoxLayout;
    date_layout2= new QHBoxLayout;
    layout_glob= new QVBoxLayout;

    date_layout1->addWidget(date_debut);
    date_layout1->addWidget(date_ecrite1);
    date_layout2->addWidget(date_fin);
    date_layout2->addWidget(date_ecrite2);
    layout_glob->addLayout(date_layout1);
    layout_glob->addLayout(date_layout2);

    generateur_resultat_viewer = new QPushButton("Générer mon document de résultat", this);
    layout_glob->addWidget(generateur_resultat_viewer);
    this->setLayout(layout_glob);

    connect(generateur_resultat_viewer, SIGNAL(clicked()), this, SLOT(generate()));
}

/// \fn formulaire_date::generate
/// \brief Générer le compte de résultats
/// affiche le widget de compte de resultat


void formulaire_date::generate(){
    // récupération des dates entrées par l'utilisateur.
    d1=date_ecrite1->date();
    d2=date_ecrite2->date();
    qDebug()<< "Voici la première date" << get_date_debut();
    qDebug()<< "Voici la deuxième date"<< get_date_fin();
    if(d1>d2){
        QMessageBox::warning(this,"Erreur de date","Veuillez rentrer une date de début inférieure à celle de fin.");
        close();
        return;
    } else {
        compte_resultat_viewer *c_r= new compte_resultat_viewer(get_date_debut(),get_date_fin());
        c_r->setAttribute(Qt::WA_DeleteOnClose);
        c_r->show();
        close();
    }
}
