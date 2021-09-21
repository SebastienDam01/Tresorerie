#include "compte_resultat_viewer.h"
#include "transaction.h"

/// \file compte_resultat_viewer.cpp
/// \brief Fichiers de fonctions pour la classe compte_resultat_viewer

/// \fn compte_resultat_viewer::compte_resultat_viewer
/// \brief Constructeur de la classe compte_resultat_viewer qui permet de visualiser dans un nouveau widget le compte de résultat sur une période donnée par l'utilisateur, le constructeur crée directement le document en prenant en compte les données saisies au préalable par l'utilisateur
/// \param d1 la date de début
/// \param d2 la date de fin
/// \param parent le widget parent

compte_resultat_viewer::compte_resultat_viewer(QDate d1, QDate d2, QWidget *parent) : QWidget(parent)
{
    qDebug()<< "Voici les dates reçues" << d1 << d2;
    this->setWindowTitle(QString("Mon compte de résultat"));
    // Création des deux QTableWidget;
    // Création du QTableWidget;
    bilan_recettes=new QTableWidget(0,2); // 2 colonnes
    bilan_recettes->setColumnWidth(0,249);
    bilan_recettes->setColumnWidth(1,149);
    // couleur d’écriture
    bilan_recettes->horizontalHeader()->setStyleSheet("color: black");
    // rendre la table non modifiable
    bilan_recettes->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // rendre les labels des lignes invisibles
    bilan_recettes->verticalHeader()->setVisible(false);
    QStringList HHeaderList;
    HHeaderList<<"Comptes"<<"Solde";
    // affectation de la liste de label au Header vertical
    bilan_recettes->setHorizontalHeaderLabels(HHeaderList);
    //largeur fixe
    bilan_recettes->setFixedWidth(400);
    bilan_recettes->setFixedHeight(250);

    bilan_depenses=new QTableWidget(0,2); // 2 colonnes
    // couleur d’écriture
    bilan_depenses->setColumnWidth(0,249);
    bilan_depenses->setColumnWidth(1,149);
    bilan_depenses->horizontalHeader()->setStyleSheet("color: black");
    // rendre la table non modifiable
    bilan_depenses->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // rendre les labels des lignes invisibles
    bilan_depenses->verticalHeader()->setVisible(false);
    QStringList HHeaderList2;
    HHeaderList2<<"Comptes"<<"Solde";
    // affectation de la liste de label au Header vertical
    bilan_depenses->setHorizontalHeaderLabels(HHeaderList2);
    //largeur fixe
    bilan_depenses->setFixedWidth(400);
    bilan_depenses->setFixedHeight(250);

    total_recettes= new QLabel("Total Recettes");
    total_depenses= new QLabel("Total Dépenses");

    solde= new QLineEdit;
    solde->setReadOnly(true);
    solde_recette= new QLineEdit;
    solde_recette->setReadOnly(true);
    solde_depense= new QLineEdit;
    solde_depense->setReadOnly(true);

    list<Compte_racine*> liste_recettes;
    list<Compte_racine*> liste_depenses;

    CompteManager & cm = CompteManager::getManager();
    list<Compte_racine*> comptes_classiques=cm.get_liste_all_comptes();
    list<Compte_racine*> comptes_virtuels=cm.get_liste_comptes_virtuels();
    //Pour les types, 0:actifs, 1:passifs
    // D'abord avec les comptes classiques

    transactionManager &tm=transactionManager::getManager();
    bool trouve=false;
    double solde_recettes=0;
    double solde_depenses=0;
    // REMPLISSAGE DES LISTES_DEPENSES & RECETTES AVEC LES COMPTES CLASSIQUES POSSEDANT DES TRANSACTIONS DANS LA BONNE PERIODE
    // Aucune distinction entre le fait qu'ils aient un parent ou non.
    for(auto &it:comptes_classiques){
        trouve=false;
        if(it->getType()==Depenses){
            for (auto &transac:tm.get_tab_transactions()){
                trouve=false;
                QDate test_date=transac->get_date();
                // si la transaction a été effectuée dans la période donnée
                if(test_date>=d1 && test_date<=d2){
                    for(auto &triplet:transac->get_tab_triplet() ){
                        // On regarde dans les triplets correspondant à cettre transaction si le compte sur lequel on est est présent
                        if(triplet.get_compte().getNom()==it->getNom() && trouve==false){
                            qDebug()<< "Nom du compte posé" << it->getNom();
                            auto deja = std::find(liste_depenses.begin(),liste_depenses.end(),it);
                            if(deja == liste_depenses.end())
                                liste_depenses.push_back(it);
                            if(triplet.get_credit()!=0){
                                qDebug()<< "Credit dans depenses " << triplet.get_credit();
                                solde_depenses-=triplet.get_credit();
                            }
                            else{
                                qDebug()<< "Debit dans depenses " << triplet.get_debit();
                                solde_depenses+=triplet.get_debit();
                            }
                            qDebug()<< "solde_recettes" << solde_recettes;
                            trouve=true;
                            break;
                        }
                    }
                }
            }
        }
        else if(it->getType()==Recettes){
            for (auto &transac:tm.get_tab_transactions()){
                trouve=false;
                QDate test_date=transac->get_date();
                // si la transaction a été effectuée dans la période donnée
                if(test_date>=d1 && test_date<=d2){
                    for(auto &triplet:transac->get_tab_triplet()){
                        // On regarde dans les triplets correspondant à cettre transaction si le compte sur lequel on est est présent
                        if(triplet.get_compte().getNom()==it->getNom() && trouve==false){
                            qDebug()<< "Nom du compte posé" << it->getNom();
                            auto deja = std::find(liste_recettes.begin(),liste_recettes.end(),it);
                            if(deja == liste_recettes.end())
                                liste_recettes.push_back(it);

                            if(triplet.get_credit()!=0){
                                solde_recettes+=triplet.get_credit();
                            }
                            else{
                                solde_recettes-=triplet.get_debit();
                            }
                            qDebug()<< "solde_recettes" << solde_recettes;
                            trouve=true;
                            break;
                        }
                    }
                }
            }
        }
    }
    // ON REMPLIT NOS LISTES DEPENSES ET RECETTES AVEC DES COMPTES VIRTUELS DEPENSES ET RECETTES
    for(auto &it:comptes_virtuels){
        trouve=false;
        if(it->getType()==Depenses){
            liste_depenses.push_back(it);
        }
        else if(it->getType()==Recettes){
            liste_recettes.push_back(it);
        }
    }
    if(liste_recettes.empty()==true && liste_depenses.empty()==true){
        QMessageBox::warning(this,"Exception 0 recette ou depenses","Attention, veuillez enregistrer une transaction pour au moins un compte de recette ou dépenses!");
        //return;
    }
    int w=0;
    double temp;
    QString val;

    // RECETTES
    for(auto &it:liste_recettes){
        bool trouve=false;
        if(it->IsVirtuel()){
            bilan_recettes->insertRow(w);
            bilan_recettes->setItem(w,0, new QTableWidgetItem(it->getNom()+" [Virtuel]"));
            w++;
            CompteVirtuel *c_v=dynamic_cast<CompteVirtuel *> (it);
            for(auto &liste_fils:c_v->recup_children()){ // La liste d'enfant ne vérifie pas la période, on doit revérifier.
                auto dans_periode=std::find(liste_recettes.begin(), liste_recettes.end(), liste_fils);
                if(!liste_fils->IsVirtuel() && dans_periode!=liste_recettes.end()){ // si le compte enfant n'est pas virtuel et est bien dans la période voulue.
                    bilan_recettes->insertRow(w);
                    bilan_recettes->setItem(w,0, new QTableWidgetItem(liste_fils->getNom()));
                    double solde_periode=0;
                    // Pour chaque transaction, on regarde si le compte en question est présent dans le triplet.
                    for(auto &verif_transac:transactionManager::getManager().get_tab_transactions()){ // on regarde dans les transactions
                        bool trouve=false;
                        QDate test_date=verif_transac->get_date();
                        // si la transaction a été effectuée dans la période donnée
                        if(test_date>=d1 && test_date<=d2){
                            for(auto &triplet:verif_transac->get_tab_triplet() ){
                                // On regarde dans les triplets correspondant à cettre transaction si le compte sur lequel on est est présent
                                if(triplet.get_compte().getNom()==liste_fils->getNom() && trouve==false){
                                    if(triplet.get_credit()!=0){
                                        solde_periode+=triplet.get_credit();
                                    }
                                    else{
                                        solde_periode-=triplet.get_debit();
                                    }
                                    trouve=true;
                                }
                            }
                        }
                    }
                    qDebug() << "Solde : " << solde_periode << "Compte " << it->getNom();
                    val.setNum(solde_periode);
                    bilan_recettes->setItem(w,1, new QTableWidgetItem(val));
                    w++;
                }
            }
        }
        else if(!it->IsVirtuel() && it->getParent()==nullptr){ // liste de comptes de dépenses classiques sans pere
            bilan_recettes->insertRow(w);
            bilan_recettes->setItem(w,0, new QTableWidgetItem(it->getNom()));
            double solde_periode=0;
            // Pour chaque transaction, on regarde si le compte en question est présent dans le triplet.
            for(auto &verif_transac:transactionManager::getManager().get_tab_transactions()){
                QDate test_date=verif_transac->get_date();
                // si la transaction a été effectuée dans la période donnée
                if(test_date>=d1 && test_date<=d2){
                    for(auto &triplet:verif_transac->get_tab_triplet() ){
                        // On regarde dans les triplets correspondant à cettre transaction si le compte sur lequel on est est présent
                        if(triplet.get_compte().getNom()==it->getNom()){
                            if(triplet.get_credit()!=0){
                                solde_periode+=triplet.get_credit();
                            }
                            else{
                                solde_periode-=triplet.get_debit();
                            }
                        }
                    }
                }
            }
            val.setNum(solde_periode);
            bilan_recettes->setItem(w,1, new QTableWidgetItem(val));
            w++;
        }
    }
    for(; w<bilan_recettes->rowCount(); w++) bilan_recettes->hideRow(w);

    /* GESTION DES DEPENSES */

    w=0;
    for(auto &it:liste_depenses){
        bool trouve=false;
        if(it->IsVirtuel()){
            bilan_depenses->insertRow(w);
            bilan_depenses->setItem(w,0, new QTableWidgetItem(it->getNom()+" [Virtuel]"));
            w++;
            CompteVirtuel *c_v=dynamic_cast<CompteVirtuel *> (it);
            double solde_periode=0;
            for(auto &liste_fils:c_v->recup_children()){
                auto dans_periode=std::find(liste_depenses.begin(), liste_depenses.end(), liste_fils);
                if(!liste_fils->IsVirtuel() && dans_periode!=liste_depenses.end()){ // liste de comptes de dépenses classiques sans pere
                    bilan_depenses->insertRow(w);
                    bilan_depenses->setItem(w,0, new QTableWidgetItem(liste_fils->getNom()));
                    double solde_periode=0;
                    // Pour chaque transaction, on regarde si le compte en question est présent dans le triplet.
                    for(auto &verif_transac:transactionManager::getManager().get_tab_transactions()){
                        bool trouve=false;
                        QDate test_date=verif_transac->get_date();
                        // si la transaction a été effectuée dans la période donnée
                        if(test_date>=d1 && test_date<=d2){
                            for(auto &triplet:verif_transac->get_tab_triplet() ){
                                // On regarde dans les triplets correspondant à cettre transaction si le compte sur lequel on est est présent
                                if(triplet.get_compte().getNom()==liste_fils->getNom()){
                                    if(triplet.get_credit()!=0){
                                        solde_periode-=triplet.get_credit();
                                    }
                                    else{
                                        solde_periode+=triplet.get_debit();
                                    }
                                }
                            }
                        }
                    }
                    val.setNum(solde_periode);
                    bilan_depenses->setItem(w,1, new QTableWidgetItem(val));
                    w++;
                }
            }
        }
        else if(!it->IsVirtuel() && it->getParent()==nullptr){ // liste de comptes de dépenses classiques sans pere
            bilan_depenses->insertRow(w);
            bilan_depenses->setItem(w,0, new QTableWidgetItem(it->getNom()));
            double solde_periode=0;
            // Pour chaque transaction, on regarde si le compte en question est présent dans le triplet.
            for(auto &verif_transac:transactionManager::getManager().get_tab_transactions()){
                bool trouve=false;
                QDate test_date=verif_transac->get_date();
                // si la transaction a été effectuée dans la période donnée
                if(test_date>=d1 && test_date<=d2){
                    for(auto &triplet:verif_transac->get_tab_triplet() ){
                        // On regarde dans les triplets correspondant à cettre transaction si le compte sur lequel on est est présent
                        if(triplet.get_compte().getNom()==it->getNom() && trouve==false){
                            if(triplet.get_credit()!=0){
                                solde_periode-=triplet.get_credit();
                            }
                            else{
                                solde_periode+=triplet.get_debit();
                            }
                            trouve=true;
                        }
                    }
                }
            }
            val.setNum(solde_periode);
            bilan_depenses->setItem(w,1, new QTableWidgetItem(val));
            w++;
        }
    }
    for(; w<bilan_depenses->rowCount(); w++) bilan_depenses->hideRow(w);

    bilan_layout= new QVBoxLayout;
    solde_total_layout= new QHBoxLayout;
    solde_t_d_layout= new QHBoxLayout;
    solde_t_r_layout= new QHBoxLayout;
    QString solde_total; // solde_total contient les valeurs en double des soldes qu'il "Set" à un QString
    QString solde_d;
    QString solde_r;
    solde_d.setNum(solde_depenses);
    solde_r.setNum(solde_recettes);
    double sold=solde_recettes-solde_depenses;
    solde_total.setNum(abs(sold));
    total= new QLabel("");
    if(sold>0){
        total->setText("Bénéfices");
    }
    else if (sold<0){
        total->setText("Déficit");
    }
    else {
        total->setText("Equilibre");
    }
    solde->setText(solde_total);

    solde_total_layout->addWidget(total);
    solde_total_layout->addWidget(solde);

    solde_depense->setText(solde_d);
    solde_t_d_layout->addWidget(total_depenses);
    solde_t_d_layout->addWidget(solde_depense);


    solde_recette->setText(solde_r);
    solde_t_r_layout->addWidget(total_recettes);
    solde_t_r_layout->addWidget(solde_recette);

    bilan_layout->addWidget(bilan_recettes);
    bilan_layout->addLayout(solde_t_r_layout);
    bilan_layout->addWidget(bilan_depenses);
    bilan_layout->addLayout(solde_t_d_layout);
    bilan_layout->addLayout(solde_total_layout);

    this->setLayout(bilan_layout);
}
