#include "all_transactions.h"

/// \file all_transactions.cpp
/// \brief Fichiers de fonctions pour la classe all_transactions

/// \fn all_transactions::all_transactions
/// \brief Constructeur du Widget qui permet de visualiser les transactions sous la forme d'une table.
/// Création d'une table non modifiable pour visualiser les transactions et leurs informations
/// \param parent Le Widget Parent

all_transactions::all_transactions(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle(QString("Historique des transactions"));

    transfertsl=new QLabel("Historique de tous les transferts de l'association :");
    // Création du QTableWidget;
    historique_transac=new QTableWidget(0,6); // 6 colonnes
    // couleur d’écriture
    historique_transac->horizontalHeader()->setStyleSheet("color: black");
    // rendre la table non modifiable
    historique_transac->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // rendre les labels des lignes invisibles
    historique_transac->verticalHeader()->setVisible(false);
    QStringList HHeaderList;
    HHeaderList<<"Date"<<"Reference"<<"Titre"<<"Comptes"<<"Débit"<<"Crédit";
    // affectation de la liste de label au Header vertical
    historique_transac->setHorizontalHeaderLabels(HHeaderList);
    //largeur fixe
    historique_transac->setFixedWidth(1000);
    historique_transac->setFixedHeight(400);

    couche=new QVBoxLayout;
    couche->addWidget(transfertsl);
    couche->addWidget(historique_transac);
    this->setLayout(couche);

    qDebug()<<"nbr="<<historique_transac->rowCount()<<"\n";
    historique_transac->setCurrentCell(0,0);
    int j=0;

    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> compte_classiques= cm.get_liste_all_comptes(); // Compte_racine:: pour dire de quelle classe vient la méthode statique
    QString s;

    transactionManager &tm=transactionManager::getManager();
    list<transaction *> liste_transaction=tm.get_tab_transactions();
    liste_transaction.sort(cmp);
    for(auto it=liste_transaction.begin();it!=liste_transaction.end();it++) // Liste de toutes les transactions
    {
        for(auto &it_:(*it)->get_tab_triplet())
        {
            historique_transac->insertRow(0);
            qDebug()<<"Reference:"<<(*it)->get_reference()<<"\n";
            j++;
            QDate s1;
            QString s2,d1;
            QString ref;
            ref.setNum((*it)->get_reference());
            s1=(*it)->get_date();
            d1=s1.toString("dd/MM/yyyy");
            qDebug()<<"Date:"<<s1<<"\n";
            historique_transac->setItem(0, 0, new QTableWidgetItem(d1));
            historique_transac->setItem(0, 1, new QTableWidgetItem(ref));
            s2=(*it)->get_titre();
            qDebug()<<"Titre:"<<s2<<"\n";
            historique_transac->setItem(0, 2, new QTableWidgetItem(s2));
            QString nom;
            nom=(it_).get_compte().getNom();
            historique_transac->setItem(0, 3, new QTableWidgetItem(nom));
            qDebug()<<"Nom:"<<nom<<"\n";
            if((it_).get_credit()==0){
                QString deb;
                deb.setNum((it_).get_debit());
                historique_transac->setItem(0, 4, new QTableWidgetItem(deb));
                qDebug()<<"Debit:"<<(it_).get_debit()<<"\n";

            }
            else{
                QString cred;
                cred.setNum((it_).get_credit());
                historique_transac->setItem(0, 5, new QTableWidgetItem(cred));
                qDebug()<<"Credit:"<<(it_).get_credit()<<"\n";

            }
        }

        historique_transac->horizontalHeader()->stretchLastSection();
        historique_transac->verticalHeader()->stretchLastSection();
    }
    // cacher les lignes restantes
    for(; j<historique_transac->rowCount(); j++) historique_transac->hideRow(j);

}


/// \fn cmp
/// \brief fonction booléenne pour comparer deux transactions uniquement selon leur attribut Date.
/// \param t1 pointeur sur une transaction
/// \param t2 pointeur sur une transaction
/// \return un booleen


bool cmp(transaction * t1, transaction* t2){
    return t1->get_date()>t2->get_date();
}
