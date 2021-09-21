#include "all_rapprochements.h"
#include "transaction.h"


/// \file all_rapprochements.cpp
/// \brief Fichier de fonctions pour la classe all_rapprochements
/// \fn all_rapprochements
/// \brief Constructeur de la classe all_rapprochements
/// Création d'une table pour visualiser tous les rapprochements de comptes déjà effectués
/// \param parent Le widget parent

all_rapprochements::all_rapprochements(QWidget *parent): QWidget(parent){
    this->setWindowTitle(QString("Historique des rapprochements"));
    // Création du QTableWidget;
    mes_rapprochements=new QTableWidget(0,3); // 3 colonnes
    mes_rapprochements->setColumnWidth(0,199);
    mes_rapprochements->setColumnWidth(1,150);
    mes_rapprochements->setColumnWidth(2,149);

    // couleur d’écriture
    mes_rapprochements->horizontalHeader()->setStyleSheet("color: black");
    // rendre la table non modifiable
    mes_rapprochements->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // rendre les labels des lignes invisibles
    mes_rapprochements->verticalHeader()->setVisible(false);
    QStringList HHeaderList;
    HHeaderList<<"Date"<<"Compte"<<"Solde";
    // affectation de la liste de label au Header vertical
    mes_rapprochements->setHorizontalHeaderLabels(HHeaderList);
    //largeur fixe
    mes_rapprochements->setFixedWidth(500);
    mes_rapprochements->setFixedHeight(250);

    rapprochement=new QLabel("Voici l'etat de chaque compte rapproché lors du dernier rapprochement");

    rapprochement_layout=new QVBoxLayout;

    rapprochement_layout->addWidget(rapprochement);
    rapprochement_layout->addWidget(mes_rapprochements);

    CompteManager &cm=CompteManager::getManager();
    transactionManager &tm=transactionManager::getManager();
    int ligne=0;
    list<Compte_racine*> comptes_classiques=cm.get_liste_all_comptes();
    double solde_dernier_rapprochement;
    QDate date_temoin(0,0,0);
    bool pris;
    for(auto &it:comptes_classiques){
        qDebug()<<"Compte"<<it->getNom()<<"Date_der:"<<it->getDateDernierRapprochement();
    }
    State *pt1= new Rapproche;
    for(auto &it:comptes_classiques){
        solde_dernier_rapprochement=0;
        pris=false;
        if(it->getDateDernierRapprochement()!=date_temoin){
            // le compte a déjà été rapproché
            // allons chercher le solde initial
            pris=true;
            for(auto &it2:tm.get_tab_transactions())
            {
                if (typeid(*((it2)->get_current_state())).name() == typeid(*pt1).name())
                {
                    for(auto &it_:it2->get_tab_triplet())
                    {
                        // si on trouve une liste de triplet correspondante, on la montre à l'utilisateur et on lui demande si il veut rapprocher ou pas
                        if(it_.get_compte().getNom()==it->getNom())
                        {
                            if(it_.get_credit() > 0)
                            {
                                if(it_.get_compte().getType() == Passifs || it_.get_compte().getType() == Recettes){
                                    solde_dernier_rapprochement += it_.get_credit();
                                }
                                else{
                                    solde_dernier_rapprochement -= it_.get_credit();
                                }
                            }
                            if(it_.get_debit() > 0)
                            {
                                if(it_.get_compte().getType() == Actifs ||it_.get_compte().getType() == Depenses){
                                    solde_dernier_rapprochement += it_.get_debit();
                                }
                                else{
                                    solde_dernier_rapprochement -= it_.get_debit();
                                }
                            }
                        }
                    }
                }
            }
        }
        if(pris==true){
            // Remplissons le QTableWidget
            mes_rapprochements->insertRow(ligne);
            qDebug()<<"Date lu depuis fichier:"<<it->getDateDernierRapprochement();
            QString date_der_rap=it->getDateDernierRapprochement().toString("dd/MM/yyyy");
            mes_rapprochements->setItem(ligne,0, new QTableWidgetItem(date_der_rap));
            mes_rapprochements->setItem(ligne,1, new QTableWidgetItem(it->getNom()));
            QString val;
            val.setNum(solde_dernier_rapprochement);
            qDebug()<<"Valeur du solde"<<val;
            mes_rapprochements->setItem(ligne, 2, new QTableWidgetItem(val));
            ligne++;
        }
    }
    this->setLayout(rapprochement_layout);
}
