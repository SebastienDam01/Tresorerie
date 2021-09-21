#include "CapitauxPropres.h"


/// \file CapitauxPropres.cpp
/// \brief Fichiers de fonctions pour la classe CapitauxPropres

/// \fn capitaux_viewer::capitaux_viewer
/// \brief Contructeur de la classe capitaux_viewer le Widget qui affiche la liste des capitaux propres utilisables
/// \param c le compte de capitaux propres
/// \param parent le Widget Parent

capitaux_viewer::capitaux_viewer(Compte * c,QWidget * parent) : QWidget (parent), new_compte(c){
    setWindowTitle("Choix Capitaux Propres");
    capl = new QLabel("Veuillez choisir/créer un compte de capitaux propres");
    capitaux_propres = new QListWidget();

    //liste des comptes de capitaux propres potentiels
    CompteManager& cm = CompteManager::getManager();
    auto liste = cm.get_liste_all_comptes();
    unsigned int compt = 0;
    for(auto &it : liste)
    {
        if(it->getType() == Passifs && it->getNom() != new_compte->getNom())
        {
            capitaux_propres->addItem(it->getNom());
            compt++;
        }

    }
    selectionner = new QPushButton("Selectionner");
    ajouter = new QPushButton("Ajouter");
    layout = new QVBoxLayout();
    layout->addWidget(capl);
    layout->addWidget(capitaux_propres);
    if(compt != 0)
        layout->addWidget(selectionner);
    layout->addWidget(ajouter);
    setLayout(layout);
    QObject::connect(selectionner,SIGNAL(clicked()),this, SLOT(new_transaction()));
    QObject::connect(ajouter,SIGNAL(clicked()),this,SLOT(creer_compte_capitaux_propres()));

}

/// \fn capitaux_viewer::creer_compte_capitaux_propres
/// \brief Fonction qui crée le compte de capitaux propres
/// \return void


void capitaux_viewer::creer_compte_capitaux_propres(){
    bool cancel;
    QString nom = QInputDialog::getText(this, "nom compte", "Entrez le nom du compte de capitaux propres:", QLineEdit::Normal, "", &cancel);
    if(!cancel){
        return;
    }
    Compte c(nom,Passifs);
    this->capitaux_propres->clear();

    CompteManager& cm = CompteManager::getManager();
    auto liste = cm.get_liste_all_comptes();
    int compt = 0;
    for(auto &it : liste)
    {
        if(it->getType() == Passifs && it->getNom() != new_compte->getNom())
        {
            capitaux_propres->addItem(it->getNom());
            capitaux_propres->setCurrentRow(compt);
            compt++;
        }
    }
    if(compt == 1)
        layout->addWidget(selectionner);
}

/// \fn capitaux_viewer::new_transaction
/// \brief Permet de faire une transaction correspondant à un solde initial
/// \return void

void capitaux_viewer::new_transaction(){
    if(capitaux_propres->currentItem() != nullptr){
        bool format_mauvais=true;
        QDate date;
        while(format_mauvais){
            bool cancel;
            QString date_str = QInputDialog::getText(this, "Date", "Entrez la date de la transaction:",QLineEdit::Normal, "", &cancel);
            if(!cancel){
                return;
            }
            date = QDate::fromString(date_str, "dd/MM/yyyy");
            if(date.isValid()){
                format_mauvais=false;
            }
            else{
                QMessageBox::warning(this,"Erreur Format", "Attention, date invalide ! Veuillez respecter le format dd/MM/yyyy");
            }
        }
        transactionManager & tm = transactionManager::getManager();
        transaction t(date,tm.recupRef(),"Solde initial");
        bool cancel;
        double solde_new_compte = QInputDialog::getDouble(this, "Nouveau Compte", "Entrez le solde du nouveau compte :",0,0,1000000000000000,2, &cancel);
        if(!cancel){
            return;
        }

        CompteManager& cm = CompteManager::getManager();
        Compte_racine * cr = cm.getCompte(capitaux_propres->currentItem()->text());
        Compte * compte_capitaux = dynamic_cast<Compte *>(cr);
        if(new_compte->getType() == Actifs)
        {
            t.add_triplet(*new_compte,0,solde_new_compte);
            t.add_triplet(*compte_capitaux,solde_new_compte,0);
        }
        else if(new_compte->getType() == Passifs)
        {
            t.add_triplet(*new_compte,solde_new_compte,0);
            t.add_triplet(*compte_capitaux,0,solde_new_compte);
        }

        int valeur_retour_transac =tm.ajouterTransaction(t);

        if(valeur_retour_transac == -1){
            QMessageBox::warning(this, "Issue Date Rapprochement", "Attention, la transaction que vous voulez effectuée propose une date postérieure au dernier rapprochement d'un des comptes.");

        }
        else  if(valeur_retour_transac == -2){
            QMessageBox::warning(this, "Liste de triplets Vide", "Liste de triplets vide, impossible de créer une transaction");
        }
        else  if(valeur_retour_transac == -3){
            QMessageBox::warning(this, "Problème unique triplet", "Impossible de creer une transaction avec un unique triplet, veuillez en créer au moins un autre");

        }
        else  if(valeur_retour_transac == -4){
            QMessageBox::warning(this, "Credit et debit a 0", "Credit et debit a 0");

        }
        else  if(valeur_retour_transac == -5){
            QMessageBox::warning(this, "Credit et debit egaux", "Credit et debit egaux");

        }
        else  if(valeur_retour_transac == -6){
            QMessageBox::warning(this, "Desequilibre en debit et credit", "Desequilibre en debit et credit, veuillez revoir vos transactions");

        }
    }
    mw->MAJ_listeComptes();
    close();
}
