#include "transaction_viewer.h"
#include <QList>

/// \file transaction_viewer.cpp
/// \brief Fichiers de fonctions pour la classe transaction_viewer

/// \fn Transaction_viewer::Transaction_viewer
/// \brief Constructeur de la classe Transaction_viewer en récupérant les noms des comptes pour donner lieu a un choix sous forme de liste déroulante
/// \param parent Le Widget parent

Transaction_viewer::Transaction_viewer(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle(QString("Transfert Viewer"));
    comptel=new QLabel("Compte :");
    listeComptes=new QComboBox;
    listeComptes->setFixedWidth(200);
    listeComptes->addItem("- Choisir un compte -");

    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> compte_classiques=cm.get_liste_all_comptes(); // Compte_racine:: pour dire de quelle classe vient la méthode statique
    for(auto it=compte_classiques.begin();it!=compte_classiques.end(); it++)
    {
        QString line=(*it)->getNom();
        listeComptes->addItem(line);
    }


    soldel=new QLabel("Solde :");
    solde=new QLineEdit;
    solde->setFixedWidth(100);

    compte_layout=new QHBoxLayout;
    compte_layout->addWidget(comptel);
    compte_layout->addWidget(listeComptes);
    compte_layout->addStretch();
    compte_layout->addWidget(soldel);
    compte_layout->addWidget(solde);

    // Donner un peu de couleur au solde et empêcher son édition.
    solde->setStyleSheet("background: darkblue; color: yellow");
    solde->setReadOnly(true);



    transfertsl=new QLabel("Transferts :");

    // Création du QTableWidget;
    vueTransactions=new QTableWidget(0,6); // 6 colonnes
    // couleur d’écriture
    vueTransactions->horizontalHeader()->setStyleSheet("color: black");
    // rendre la table non modifiable
    vueTransactions->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // rendre les labels des lignes invisibles
    vueTransactions->verticalHeader()->setVisible(false);
    QStringList HHeaderList;
    HHeaderList<<"Date"<<"Reference"<<"Titre"<<"Comptes"<<"Débit"<<"Crédit";
    // affectation de la liste de label au Header vertical
    vueTransactions->setHorizontalHeaderLabels(HHeaderList);
    //largeur fixe
    vueTransactions->setFixedWidth(1000);
    vueTransactions->setFixedHeight(400);

    couche=new QVBoxLayout;
    couche->addLayout(compte_layout);
    couche->addWidget(transfertsl);
    couche->addWidget(vueTransactions);
    this->setLayout(couche);
    QPushButton * bouton_ajout = new QPushButton("Ajouter");
    QPushButton * bouton_sup = new QPushButton("Supprimer");
    QPushButton * bouton_modif = new QPushButton("Modifier");
    QHBoxLayout * bouton_layout=new QHBoxLayout;
    bouton_layout->addWidget(bouton_ajout);
    bouton_layout->addWidget(bouton_modif);
    bouton_layout->addWidget(bouton_sup);
    couche->addLayout(bouton_layout);
    QObject::connect(bouton_ajout,SIGNAL(clicked()),this,SLOT(Ajout_transaction()));
    connect(bouton_sup,SIGNAL(clicked()),this,SLOT(sup_transaction()));
    QObject::connect(bouton_modif,SIGNAL(clicked()),this,SLOT(Modif_transaction()));
    // connection du signal currentIndexChanged(int) à notre slot refresh
    connect(listeComptes,SIGNAL(currentIndexChanged(int)),this,SLOT(refresh(int)));

}

/// \fn Transaction_viewer::MAJ_liste_comptes
/// \brief Cette fonction met à jour la QComboBox qui correspond à la liste des comptes.

void Transaction_viewer::MAJ_liste_comptes(){
    listeComptes->blockSignals(true); //Plutot louche
    listeComptes->clear();

    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> compte_classiques=cm.get_liste_all_comptes();
    QStringList sl;
    sl.append(QString("- Choisir un compte -"));
    for(auto &it : compte_classiques)
    {
        qDebug()<<it->getNom();
        sl.append(QString(it->getNom()));
    }
    listeComptes->addItems(sl);
    listeComptes->blockSignals(false);
}

/// \fn Transaction_viewer::refresh
/// \brief Mettre à jour la liste des transactions dans la QTableWidget de Transactions_viewer
/// \param i index du compte dans la liste déroulante

void Transaction_viewer::refresh(int i){
    qDebug()<<"nbr="<<vueTransactions->rowCount()<<"\n";
    //On place la currentcell de vueTransactions à une place random pour pas supprimer de transaction si on clique sur supprimer et qu'on a précliquer sur une transaction
    vueTransactions->setCurrentCell(0,0);
    int j=0;
    if (i>0){
        cout << i;
        QString compte=listeComptes->itemText(i);
        qDebug()<<"Compte :"<<compte<<"\n";
        CompteManager& cm = CompteManager::getManager();
        list<Compte_racine*> compte_classiques=cm.get_liste_all_comptes(); // Compte_racine:: pour dire de quelle classe vient la méthode statique
        QString s;
        Compte *c;
        for (auto it=compte_classiques.begin(); it!=compte_classiques.end(); it++)
        {
            if((*it)->getNom()==compte)
            {
                c=dynamic_cast<Compte*>(*it);
                break;
            }
        }
        s.setNum(c->getSolde());
        solde->setText(s);
        transactionManager &tm=transactionManager::getManager();
        list<transaction *> liste_transaction=tm.get_tab_transactions();
        for(auto it=liste_transaction.begin();it!=liste_transaction.end();it++) // Liste de toutes les transactions
        {
            int present=0;
            for(auto &it_:(*it)->get_tab_triplet()){
                if ((it_).get_compte().getNom()==compte)
                {
                    present=1;
                    break;
                }

            }
            qDebug()<<"Present:"<<present<<endl;
            if(present==1){
                for(auto &it_:(*it)->get_tab_triplet())
                {
                    QString nom;
                    qDebug()<< "Compte" << compte;
                    nom=(it_).get_compte().getNom();
                    qDebug()<< "Nom compte" << nom;
                    if(nom==compte){
                        vueTransactions->insertRow(0);
                        j++;
                        QDate s1;
                        QString s2,d1;
                        QString ref;
                        ref.setNum((*it)->get_reference());
                        s1=(*it)->get_date();
                        d1=s1.toString();
                        vueTransactions->setItem(0, 0, new QTableWidgetItem(d1));
                        vueTransactions->setItem(0, 1, new QTableWidgetItem(ref));
                        s2=(*it)->get_titre();
                        vueTransactions->setItem(0, 2, new QTableWidgetItem(s2));
                        QString nom;
                        nom=(it_).get_compte().getNom();
                        vueTransactions->setItem(0, 3, new QTableWidgetItem(nom));
                        if((it_).get_credit()==0){
                            QString deb;
                            deb.setNum((it_).get_debit());
                            vueTransactions->setItem(0, 4, new QTableWidgetItem(deb));
                            qDebug()<<"Debit:"<<(it_).get_debit()<<"\n";
                        }
                        else{
                            QString cred;
                            cred.setNum((it_).get_credit());
                            vueTransactions->setItem(0, 5, new QTableWidgetItem(cred));
                            qDebug()<<"Credit:"<<(it_).get_credit()<<"\n";
                        }
                    }
                }
                vueTransactions->horizontalHeader()->stretchLastSection();
                vueTransactions->verticalHeader()->stretchLastSection();
            }
        }

    }
    else{ // pas de compte dans la liste déroulante
        solde->setText("");
    }
    // cacher les lignes restantes
    for(; j<vueTransactions->rowCount(); j++) vueTransactions->hideRow(j);
}

/// \fn Transaction_viewer::Ajout_transaction
/// \brief Ajouter une transaction depuis le transaction viewer
/// on ouvre un widget Information pour récolter les informations nécéssaires à la création de la transaction

void Transaction_viewer::Ajout_transaction(){
    info = new Information();
    //lorsque l'on ferme le widget info, on le détruit
    info->setAttribute(Qt::WA_DeleteOnClose);
    connect(info,SIGNAL(destroyed(QObject*)),mw,SLOT(MAJ_listeComptes()));
    info->show();
    refresh(0);
    listeComptes->setCurrentIndex(0);
}

/// \fn Transaction_viewer::sup_transaction
/// \brief Permet la suppression d'une transaction depuis Transaction_viewer
/// Il suffit de cliquer sur la ligne de la transaction pour selectionner la transaction à supprimer

void Transaction_viewer::sup_transaction(){
    State * pt1 = new Non_Rapproche;
    if(vueTransactions->rowCount() == 0)
        return ;
    transactionManager &tm = transactionManager::getManager();
    for(auto &it : tm.get_tab_transactions())
    {
        //On se place au bon endroit pour prendre la référence de la transaction
        vueTransactions->setCurrentCell(vueTransactions->currentRow(),1);
        if(it->get_reference() == vueTransactions->currentItem()->text().toInt())
        {
            if(typeid(*(it->get_current_state())).name() == typeid (*pt1).name())
                tm.annuler_transaction(*it);
            else
                QMessageBox::warning(this,"Suppression interdite", "Suppression interdite car la transaction a été rapprochée");
        }
    }
    refresh(listeComptes->currentIndex());
    mw->MAJ_listeComptes();
}

/// \fn Transaction_viewer::Modif_transaction
/// \brief Permet de modifier les informations d'une transaction
/// Il suffit de cliquer sur la ligne de la transaction pour selectionner la transaction à modifier

void Transaction_viewer::Modif_transaction(){
    State * pt1 = new Non_Rapproche;
    if(vueTransactions->rowCount() == 0)
        return ;
    transactionManager &tm = transactionManager::getManager();
    for(auto &it : tm.get_tab_transactions())
    {
        //On se place au bon endroit pour prendre la référence de la transaction
        vueTransactions->setCurrentCell(vueTransactions->currentRow(),1);
        if(it->get_reference() == vueTransactions->currentItem()->text().toInt())
        {
            if(typeid(*(it->get_current_state())).name() == typeid (*pt1).name())
            {
                info = new Information(it);
                info->setAttribute(Qt::WA_DeleteOnClose);
                connect(info,SIGNAL(destroyed(QObject*)),mw,SLOT(MAJ_listeComptes()));
                info->show();
                //refresh(listeComptes->currentIndex());
                refresh(0);
                listeComptes->setCurrentIndex(0);
                break;
            }
            else
                QMessageBox::warning(this,"Modification interdite", "Modification interdite car la transaction a été rapprochée");
        }
    }
}
