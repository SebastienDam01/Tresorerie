#include "bilan_viewer.h"
#include "transaction.h"

/// \file bilan_viewer.cpp
/// \brief Fichiers de fonctions pour la classe bilan_viewer

/// \fn bilan_viewer::bilan_viewer
/// \brief Constructeur de la classe bilan_viewer, le Widget qui crée le bilan comptable
/// \param parent le Widget parent


bilan_viewer::bilan_viewer(QWidget *parent) : QWidget(parent)
{

    this->setWindowTitle(QString("Mon bilan"));
    // Création des deux QTableWidget;
    // Création du QTableWidget;
    bilan_actif=new QTableWidget(0,2); // 2 colonnes
    bilan_actif->setColumnWidth(0,249);
    bilan_actif->setColumnWidth(1,149);
    // couleur d’écriture
    bilan_actif->horizontalHeader()->setStyleSheet("color: black");
    // rendre la table non modifiable
    bilan_actif->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // rendre les labels des lignes invisibles
    bilan_actif->verticalHeader()->setVisible(false);
    QStringList HHeaderList;
    HHeaderList<<"Comptes"<<"Solde";
    // affectation de la liste de label au Header vertical
    bilan_actif->setHorizontalHeaderLabels(HHeaderList);
    //largeur fixe

    bilan_actif->setFixedWidth(400);
    bilan_actif->setFixedHeight(250);
    bilan_passif=new QTableWidget(0,2); // 2 colonnes
    bilan_passif->setColumnWidth(0,249);
    bilan_passif->setColumnWidth(1,149);
    // couleur d’écriture
    bilan_passif->horizontalHeader()->setStyleSheet("color: black");
    // rendre la table non modifiable
    bilan_passif->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // rendre les labels des lignes invisibles
    bilan_passif->verticalHeader()->setVisible(false);
    QStringList HHeaderList2;
    HHeaderList2<<"Comptes"<<"Solde";
    // affectation de la liste de label au Header vertical
    bilan_passif->setHorizontalHeaderLabels(HHeaderList2);
    //largeur fixe
    bilan_passif->setFixedWidth(400);
    bilan_passif->setFixedHeight(250);

    total=new QLabel("Total [Actifs-Passifs] :");
    total_actif= new QLabel("Total Actifs");
    total_passif= new QLabel("Total Passifs");

    solde= new QLineEdit;
    solde->setReadOnly(true);
    solde_actif= new QLineEdit;
    solde_actif->setReadOnly(true);
    solde_passif= new QLineEdit;
    solde_passif->setReadOnly(true);

    list<Compte_racine*> liste_passifs;
    list<Compte_racine*> liste_actifs;
    CompteManager & cm = CompteManager::getManager();
    list<Compte_racine*> comptes_classiques=cm.get_liste_all_comptes();
    list<Compte_racine*> comptes_virtuels=cm.get_liste_comptes_virtuels();
    //Pour les types, 0:actifs, 1:passifs
    // D'abord avec les comptes classiques
    for(auto &it:comptes_classiques){
        if(it->getType()==0){
            liste_actifs.push_back(it);
        }
        else if(it->getType()==1){
            liste_passifs.push_back(it);
        }
    }
    // Puis avec les comptes virtuels
    for(auto &it:comptes_virtuels){
        if(it->getType()==0){
            liste_actifs.push_back(it);
        }
        else if(it->getType()==1){
            liste_passifs.push_back(it);
        }
    }
    int w=0;
    double solde_actifs=0;
    double solde_passifs=0;
    double temp;
    QString val;
    for(auto &it:liste_actifs){
        if(it->IsVirtuel()){
            bilan_actif->insertRow(w);
            bilan_actif->setItem(w,0, new QTableWidgetItem(it->getNom()+ " [Virtuel]"));
            temp=it->getSolde();
            val.setNum(temp);
            bilan_actif->setItem(w,1, new QTableWidgetItem(val));
            w++;
            CompteVirtuel *c_v=dynamic_cast<CompteVirtuel *> (it);
            for(auto &liste_fils:c_v->recup_children()){
                if(!liste_fils->IsVirtuel()){
                    bilan_actif->insertRow(w);
                    bilan_actif->setItem(w,0, new QTableWidgetItem(liste_fils->getNom()));
                    temp=liste_fils->getSolde();
                    val.setNum(temp);
                    bilan_actif->setItem(w,1, new QTableWidgetItem(val));
                    w++;
                }
            }
        }
        if(it->IsVirtuel() && it->getParent()==nullptr){
            solde_actifs+=it->getSolde(); // On utilise que le solde des comptes virtuels, évite de compter deux fois.
        }
        if(!it->IsVirtuel() && it->getParent()==nullptr){
            bilan_actif->insertRow(w);
            bilan_actif->setItem(w,0, new QTableWidgetItem(it->getNom()));
            temp=it->getSolde();
            val.setNum(temp);
            bilan_actif->setItem(w,1, new QTableWidgetItem(val));
            solde_actifs+=it->getSolde();
            w++;

        }
    }

    w=0;

    for(auto &it:liste_passifs){
        if(it->IsVirtuel()){
            bilan_passif->insertRow(w);
            bilan_passif->setItem(w,0, new QTableWidgetItem(it->getNom()+ " [Virtuel]"));
            temp=it->getSolde();
            val.setNum(temp);
            bilan_actif->setItem(w,1, new QTableWidgetItem(val));
            w++;
            CompteVirtuel *c_v=dynamic_cast<CompteVirtuel *> (it);
            for(auto &liste_fils:c_v->recup_children()){
                if(!liste_fils->IsVirtuel()){
                    bilan_passif->insertRow(w);
                    bilan_passif->setItem(w,0, new QTableWidgetItem(liste_fils->getNom()));
                    temp=liste_fils->getSolde();
                    val.setNum(temp);
                    bilan_passif->setItem(w,1, new QTableWidgetItem(val));
                    w++;
                }
            }
        }
        if(it->IsVirtuel() && it->getParent()==nullptr){
            solde_passifs+=it->getSolde(); // On utilise que le solde des comptes virtuels, évite de compter deux fois.
        }
        if(!it->IsVirtuel() && it->getParent()==nullptr){
            bilan_passif->insertRow(w);
            bilan_passif->setItem(w,0, new QTableWidgetItem(it->getNom()));
            temp=it->getSolde();
            val.setNum(temp);
            bilan_passif->setItem(w,1, new QTableWidgetItem(val));
            solde_passifs+=it->getSolde();
            w++;
        }

    }
    bilan_layout= new QVBoxLayout;
    solde_total_layout= new QHBoxLayout;
    solde_t_a_layout= new QHBoxLayout;
    solde_t_p_layout= new QHBoxLayout;
    QString solde_total; // solde_total contient les valeurs en double des soldes qu'il "Set" à un QString
    QString solde_p;
    QString solde_a;
    solde_a.setNum(solde_actifs);
    solde_p.setNum(solde_passifs);
    double sold=solde_actifs-solde_passifs;
    solde_total.setNum(sold);

    qDebug()<<"Solde"<<solde_total;

    solde->setText(solde_total);

    solde_total_layout->addWidget(total);
    solde_total_layout->addWidget(solde);

    solde_actif->setText(solde_a);
    solde_t_a_layout->addWidget(total_actif);
    solde_t_a_layout->addWidget(solde_actif);


    solde_passif->setText(solde_p);
    solde_t_p_layout->addWidget(total_passif);
    solde_t_p_layout->addWidget(solde_passif);

    bilan_layout->addWidget(bilan_actif);
    bilan_layout->addLayout(solde_t_a_layout);
    bilan_layout->addWidget(bilan_passif);
    bilan_layout->addLayout(solde_t_p_layout);
    bilan_layout->addLayout(solde_total_layout);
    this->setLayout(bilan_layout);
}
