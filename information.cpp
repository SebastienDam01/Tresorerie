#include "information.h"

 /// \file informations.cpp
 /// \brief Fichiers de fonctions pour la classe informations

 /// \fn Information::Information
 /// \brief Constructeur de la classe Information
 /// \param tr la transaction à laquelle vont servir les informations du formulaire
 /// \param parent
 /// \return Une instance de la classe Information

Information::Information(transaction * tr ,QWidget *parent) : QWidget (parent){
    numero = 1;
    old = tr;
    setWindowTitle(QString("Date et titre de la transaction"));
    datel = new QLabel("Entrer la date :");
    titrel = new QLabel("Entrer le titre :");
    comptel = new QLabel("Entrer le compte :");
    credl = new QLabel("Entrer le crédit :");
    debl = new QLabel("Entrer le débit :");
    date = new QDateEdit();
    titre = new QLineEdit();

    compte = new QComboBox();

    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> compte_classiques=cm.get_liste_all_comptes(); // Compte_racine:: pour dire de quelle classe vient la méthode statique
    for(auto it=compte_classiques.begin();it!=compte_classiques.end(); it++)
    {
        QString line=(*it)->getNom();
        compte->addItem(line);
    }

    cred = new QDoubleSpinBox();
    cred->setRange(0,10000000);
    deb = new QDoubleSpinBox();
    deb->setRange(0,10000000);
    confirmer = new QPushButton("Confirmer");
    suivant = new QPushButton("Nouveau triplet");
    date_layout = new QHBoxLayout();
    date_layout->addWidget(datel);
    date_layout->addWidget(date);
    titre_layout = new QHBoxLayout();
    titre_layout->addWidget(titrel);
    titre_layout->addWidget(titre);
    debit_layout = new QHBoxLayout();
    credit_layout = new QHBoxLayout();
    compte_layout = new QHBoxLayout();
    couche = new QVBoxLayout();
    couche->addLayout(date_layout);
    couche->addLayout(titre_layout);
    boutons = new QHBoxLayout();
    boutons->addWidget(suivant);
    couche->addLayout(boutons);
    date->setFixedWidth(350);
    titre->setFixedWidth(350);
    setLayout(couche);
    QObject::connect(suivant,SIGNAL(clicked()),this,SLOT(suite_transaction()));
    if(tr != nullptr){ //Modif, on ajoute des champs par défaut
        date->setDate(tr->get_date());
        titre->setText(tr->get_titre());
    }

}

 /// \fn Information::suite_transaction
 /// \brief affichage d'une nouvelle page de demande d'informations
 /// \return void

void Information::suite_transaction()
{
    setWindowTitle(QString("Compte numéro "  + QString::number(numero)) + " de la transaction");
    if(numero == 1)
    {
        QDate d = date->date();
        QString titr = titre->text();
        transactionManager & tm = transactionManager::getManager();
        if(old != nullptr)//Modif
        {
            int ref = old->get_reference();
            nouv = new transaction(d,ref,titr);
            //On met des champs par défauts (correspondant a la transaction qu'on modifie
            compte->setCurrentText(old->get_tab_triplet().begin()->get_compte().getNom());
            //compte->setText(old->get_tab_triplet().begin()->get_compte().getNom());
            deb->setValue(old->get_tab_triplet().begin()->get_debit());
            cred->setValue(old->get_tab_triplet().begin()->get_credit());
        }
        else //Ajout
            nouv = new transaction(d,tm.recupRef(),titr);

        date_layout->removeWidget(datel);
        date_layout->removeWidget(date);
        titre_layout->removeWidget(titrel);
        titre_layout->removeWidget(titre);
        delete datel;
        delete date;
        delete titrel;
        delete titre;
        compte_layout->addWidget(comptel);
        compte_layout->addWidget(compte);
        debit_layout->addWidget(debl);
        debit_layout->addWidget(deb);
        credit_layout->addWidget(credl);
        credit_layout->addWidget(cred);
        couche->removeItem(date_layout);
        couche->removeItem(titre_layout);
        delete date_layout;
        delete titre_layout;
        couche->addLayout(compte_layout);
        couche->addLayout(debit_layout);
        couche->addLayout(credit_layout);
        couche->addWidget(suivant);
        compte->setFixedWidth(350);
        deb->setFixedWidth(350);
        cred->setFixedWidth(350);
        setLayout(couche);
    }
    else
    {
        QString nom_compte = compte->currentText();
        compte->removeItem(compte->currentIndex()); //On ne peut pas avoir 2 fois le mm compte ds 2 triplets
        double debit = deb->value();
        double credit = cred->value();
        if(old != nullptr and old->get_tab_triplet().size() >= numero){
            list<triplet_transaction>::iterator it;
            it = old->get_tab_triplet().begin();
            for(int i = 1; i < numero; i++)
            {
                ++it;
            }
            compte->setCurrentText(it->get_compte().getNom());
            double debit_value = it->get_debit();
            double credit_value = it->get_credit();
            deb->setValue(debit_value);
            cred->setValue(credit_value);
        }
        else{
            //compte->setText("");
            deb->setValue(0);
            cred->setValue(0);
        }
        CompteManager& cm = CompteManager::getManager();
        Compte_racine * cr = cm.getCompte(nom_compte);
        qDebug()<<"nom de compte "<<nom_compte;
        qDebug()<<"debit "<<debit;
        qDebug()<<"credit "<<credit;
        if(cr == nullptr)
        {
            return;
        }
        Compte * c = dynamic_cast<Compte *>(cr);
        nouv->add_triplet(*c,credit,debit);
        couche->addWidget(confirmer);
        setLayout(couche);
        if(numero == 2){
            QObject::connect(confirmer,SIGNAL(clicked()),this,SLOT(envoie_info()));
        }
    }
    numero++;

}

 /// \fn Information::envoie_info
 /// \brief Envoyer les informations recueuillies dans le formulaire <=> ajout de la transaction si elle est conforme
 /// \return void

void Information::envoie_info(){
    QString nom_compte = compte->currentText();
    double debit = deb->value();
    double credit = cred->value();

    CompteManager& cm = CompteManager::getManager();
    Compte_racine * cr = cm.getCompte(nom_compte);
    if(cr != nullptr)
    {
        Compte & c = dynamic_cast<Compte &>(*cr);
        nouv->add_triplet(c,credit,debit);  //ajout du dernier triplet
    }
    transactionManager & tm = transactionManager::getManager();
    int valeur_retour_transac = tm.ajouterTransaction(*nouv);
    if(valeur_retour_transac == 0) //L'ajout a marché, les infos rentrés sont cohérentes
    {
        if(old != nullptr){
            tm.annuler_transaction(*old);
            QMessageBox::information(this, "Modification", "La transaction a été modifiée");
        }
        else{
            QMessageBox::information(this, "Ajout", "La transaction a bien été ajoutée");
        }
    }
    else  if(valeur_retour_transac == -1){
        QMessageBox::warning(this, "Issue Date Rapprochement", "Attention, la transaction que vous voulez effectuée propose une date inférieure au dernier rapprochement d'un des comptes.");

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
    close();
}
