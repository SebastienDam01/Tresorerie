#include "rapprochement.h"

/// \file rapprochement.cpp
///  \brief Fichiers de fonctions pour la classe rapprochement

/// \fn Rapprochement::Rapprochement
/// \brief Construire un widget de rapprochement depuis un bouton cliqué depuis la MainWindow
/// \param parent le widget parent
/// \param nom le nom du compte à rapprocher
/// \param mwindow la mainwindow depuis laquelle on lance le rapprochement

Rapprochement::Rapprochement(QWidget *parent, const QString& nom, MainWindow *mwindow) : QWidget (parent){
    mw = mwindow;
    decision_rapproche = 0;
    nom_compte = nom;
    setWindowTitle(QString("Rapprochement"));

    affiche = new QLabel("Voici le solde après le dernier rapprochement: ");
    solde_der_rapp = new QLineEdit;
    solde_der_rapp->setFixedWidth(100);
    solde_der_rapp->setReadOnly(true);

    affiche_solde_actu = new QLabel("Voici le solde actuel: ");
    solde_actuel = new QLineEdit;
    solde_actuel->setFixedWidth(100);
    solde_actuel->setReadOnly(true);

    question = new QLabel("Voulez-vous rapprocher ?");
    oui = new QPushButton("Oui");
    non = new QPushButton("Non");
    oui_non= new QHBoxLayout;
    oui_non->addWidget(oui);
    oui_non->addWidget(non);
    quitter=new QPushButton("Quitter fenêtre");

    affiche_layout = new QHBoxLayout();
    solde_actuel_layout = new QHBoxLayout();
    msgbox = new QVBoxLayout();

    affiche_layout->addWidget(affiche);
    affiche_layout->addWidget(solde_der_rapp);
    solde_actuel_layout->addWidget(affiche_solde_actu);
    solde_actuel_layout->addWidget(solde_actuel);
    msgbox->addWidget(question);
    couche = new QVBoxLayout();

    couche->addLayout(affiche_layout);
    couche->addLayout(solde_actuel_layout);
    couche->addLayout(msgbox);
    couche->addLayout(oui_non);
    couche->addWidget(quitter);
    setLayout(couche);
    premier_affichage = true;
    QObject::connect(oui,SIGNAL(clicked()),this,SLOT(setOui_rapproche()));
    QObject::connect(non,SIGNAL(clicked()),this,SLOT(setNon_rapproche()));
    QObject::connect(quitter, SIGNAL(clicked()), this, SLOT(Fermer_Fenetre()));
}

/// \fn Rapprochement::setSolde
/// \brief Modifier le solde du compte après une validation de transaction
/// \param solde le solde à régler

void Rapprochement::setSolde(const double& solde) const {
    QString solde_str = QString::number(solde);
    solde_der_rapp->setText(solde_str);
}

/// \fn Rapprochement::setSolde_actuel
/// \brief Modifier le solde actuel du compte concerné par le rapprochement
/// \param solde le solde à régler

void Rapprochement::setSolde_actuel(const double& solde) const {
    QString solde_str = QString::number(solde);
    solde_actuel->setText(solde_str);
}

/// \fn Rapprochement::Fermer_Fenetre
/// \brief Fermer le widget de rapprochement


void Rapprochement::Fermer_Fenetre(){
    verification=true;
    close();
}

/// \fn Rapprochement::setOui_rapproche
/// \brief Passage de toutes les transactions du compte sélectionné à l'état Rapproche.

void Rapprochement::setOui_rapproche() {
    premier_affichage = false;
    decision_rapproche = 1;
    State *pt2= new Non_Rapproche;
    transactionManager &tm=transactionManager::getManager();
    int trouve=0;
    for(auto &it : tm.get_tab_transactions()){
        if(typeid(*it->get_current_state()).name() == typeid(*pt2).name()){
            qDebug() << "typeid :" << typeid(*it->get_current_state()).name();
            for(auto &it_: it->get_tab_triplet())
            {
                // si on trouve une liste de triplet correspondante, on la montre à l'utilisateur et on lui demande si il veut rapprocher ou pas
                if(it_.get_compte().getNom()==nom_compte)
                {
                    qDebug() << "Rentre" << it_.get_compte().getNom() << nom_compte;
                    trouve = 1;
                    cout << "Voici la transaction reference " << it->get_reference() << endl;
                    if (typeid(*it->get_current_state()).name() == typeid(*pt2).name()){
                        qDebug() << "Rapproché";
                        it->get_current_state()->setState(*it, new Rapproche);
                    } else{
                        qDebug() << "Transaction déjà rapproché";
                    }
                }
            }
        }
    }
    qDebug() << "rep:" << getDecision_rapproche();
    QMessageBox msgBox;
    msgBox.setText("L'opération est terminée.");
    msgBox.exec();
    CompteManager & cm = CompteManager::getManager();
    Compte * c = dynamic_cast<Compte *>(cm.getCompte(nom_compte));
    c->setDateDernierRapprochement(QDate::currentDate());
    verification = true;

    close();
}

/// \fn Rapprochement::closeEvent
/// \brief Surcharge de closeEvent. L'utilisateur doit rapprocher toutes les transactions pour pouvoir fermer la fenêtre de Rapprochement.

void Rapprochement::closeEvent(QCloseEvent *event){
    if(verification==false && premier_affichage == false){
        int reponse=QMessageBox::question(this, "Message rapprochement","Pour quitter cette fenêtre, vous devenez finir de tout rapprocher, voulez-vous tout rapprocher maintenant ?", QMessageBox::Yes | QMessageBox::No);
        if(reponse==QMessageBox::Yes){
            setOui_rapproche();
        }
        if(reponse==QMessageBox::No){
            event->ignore();
        }
    }
    else{
        event->accept();
    }
}

/// \fn Rapprochement::setNon_rapproche
/// \brief Sélection des transactions à modifier par l'utilisateur et entrée du premier triplet.

void Rapprochement::setNon_rapproche() {
    premier_affichage = false;
    delete affiche_layout;
    delete solde_actuel_layout;
    delete msgbox;
    delete affiche;
    delete affiche_solde_actu;
    delete solde_der_rapp;
    delete solde_actuel;
    delete question;
    delete oui;
    delete non;
    delete quitter;
    decision_rapproche = 0;
    //afficher_transac = new QPushButton("Appuyez pour afficher les transactions non rapprochées.");
    rapprocher_all = new QPushButton("Appuyer pour rapprocher l'ensemble des transactions");
    setWindowTitle(QString("Création d'une nouvelle transaction"));
    question2 = new QLabel("Quelle est la transaction que vous voulez modifier ?");
    ref_trans = new QComboBox;

    quest_ref_layout = new QHBoxLayout();
    quest_ref_layout->addWidget(question2);
    quest_ref_layout->addWidget(ref_trans);

    //connect(afficher_transac,SIGNAL(clicked()),this,SLOT(refresh()));
    QObject::connect(rapprocher_all,SIGNAL(clicked()),this,SLOT(setOui_rapproche()));
    //Affichage des transactions non rapprochées

    transfertsl=new QLabel("Toutes les transactions non rapprochées sont les suivantes :");

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

    State *pt2= new Non_Rapproche;
    qDebug()<<"nbr="<<vueTransactions->rowCount()<<"\n";
    int j=0;
    transactionManager &tm=transactionManager::getManager();
    list<transaction *> liste_transaction=tm.get_tab_transactions();
    for(auto it=liste_transaction.begin();it!=liste_transaction.end();it++) // Liste de toutes les transactions
    {
        if(typeid(*(*it)->get_current_state()).name() == typeid((*pt2)).name()){
            int k=0;
            int present=0;
            for(auto &it_:(*it)->get_tab_triplet()){
                if ((it_).get_compte().getNom()==nom_compte)
                {
                    present=1;
                    break;
                }

            }
            qDebug()<<"Present:"<<present<<endl;
            //HHeaderList<<"Date"<<"Reference"<<"Titre"<<"Comptes"<<"Débit"<<"Crédit";
            if(present==1){
                ref_trans->addItem(QString::number((*it)->get_reference()));
                for(auto &it_:(*it)->get_tab_triplet())
                {

                    vueTransactions->insertRow(0);
                    cout << 3;
                    qDebug()<<"Reference:"<<(*it)->get_reference()<<"\n";
                    j++;

                    QDate s1;
                    QString s2,d1;
                    QString ref;
                    ref.setNum((*it)->get_reference());
                    s1=(*it)->get_date();
                    d1=s1.toString();
                    qDebug()<<"Date:"<<s1<<"\n";
                    vueTransactions->setItem(0, 0, new QTableWidgetItem(d1));
                    vueTransactions->setItem(0, 1, new QTableWidgetItem(ref));
                    s2=(*it)->get_titre();
                    qDebug()<<"Titre:"<<s2<<"\n";
                    vueTransactions->setItem(0, 2, new QTableWidgetItem(s2));
                    QString nom;
                    nom=(it_).get_compte().getNom();
                    vueTransactions->setItem(0, 3, new QTableWidgetItem(nom));
                    qDebug()<<"Nom:"<<nom<<"\n";
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
                    k++;
                }

                vueTransactions->horizontalHeader()->stretchLastSection();
                vueTransactions->verticalHeader()->stretchLastSection();
            }
        }

    }
    // cacher les lignes restantes
    for(; j<vueTransactions->rowCount(); j++) vueTransactions->hideRow(j);

    if(vueTransactions->rowCount() == 0)
    {
        QMessageBox::information(this,"Déjà rapproché","Toutes les transactions associées à ce compte ont été rapprochées, ce message a pour seul effet de mettre à jour la date de dernier rapprochement.");
        setOui_rapproche();
        return;
    }


    liste_transac = new QVBoxLayout();
    liste_transac->addWidget(transfertsl);
    liste_transac->addWidget(vueTransactions);

    //-fin affichage

    comptel = new QLabel("Ecrire le nom du compte :");
    credl = new QLabel("Entrer le crédit :");
    debl = new QLabel("Entrer le débit :");
    compte = new QComboBox;
    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> compte_classiques=cm.get_liste_all_comptes();
    for(auto it=compte_classiques.begin();it!=compte_classiques.end(); it++)
    {
        QString line=(*it)->getNom();
        compte->addItem(line);
    }
    cred = new QDoubleSpinBox();
    deb = new QDoubleSpinBox();
    //compte->setText("");
    deb->setRange(0,10000000000000);
    deb->setValue(0);
    cred->setRange(0,10000000000000);
    cred->setValue(0);

    debit_layout = new QHBoxLayout();
    credit_layout = new QHBoxLayout();
    compte_layout = new QHBoxLayout();

    valider_triplet_layout = new QHBoxLayout();

    valider_triplet = new QPushButton("Valider le triplet");
    valider_triplet_layout->addWidget(valider_triplet);



    compte_layout->addWidget(comptel);
    compte_layout->addWidget(compte);
    debit_layout->addWidget(debl);
    debit_layout->addWidget(deb);
    credit_layout->addWidget(credl);
    credit_layout->addWidget(cred);

    couche->addLayout(quest_ref_layout);
    couche->addLayout(liste_transac);
    couche->addWidget(rapprocher_all);
    couche->addLayout(compte_layout);
    couche->addLayout(debit_layout);
    couche->addLayout(credit_layout);
    couche->addLayout(valider_triplet_layout);

    compte->setFixedWidth(350);
    deb->setFixedWidth(350);
    cred->setFixedWidth(350);
    setLayout(couche);

    QObject::connect(valider_triplet,SIGNAL(clicked()),this,SLOT(valider_rap_triplet()));

}


/// \fn Rapprochement::setNon_rappV2
/// \brief Sélection des transactions à modifier par l'utilisateur et entrée du premier triplet.
/// cette fonction ne va pas delete les mêmes widgets que setNon_rapproche ainsi nous avons besoin de cette seconde fonction pour faire une "boucle".
void Rapprochement::setNon_rappV2(){
    decision_rapproche = 0;
    rapprocher_all = new QPushButton("Appuyer pour rapprocher l'ensemble des transactions");
    setWindowTitle(QString("Création d'une nouvelle transaction"));
    question2 = new QLabel("Quelle est la transaction que vous voulez modifier ?");
    ref_trans = new QComboBox;

    quest_ref_layout = new QHBoxLayout();
    quest_ref_layout->addWidget(question2);
    quest_ref_layout->addWidget(ref_trans);


    QObject::connect(rapprocher_all,SIGNAL(clicked()),this,SLOT(setOui_rapproche()));
    //Affichage des transactions non rapprochées

    transfertsl=new QLabel("Toutes les transactions non rapprochées sont les suivantes :");

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

    liste_transac = new QVBoxLayout();
    liste_transac->addWidget(transfertsl);
    liste_transac->addWidget(vueTransactions);

    //-fin affichage

    comptel = new QLabel("Ecrire le nom du compte :");
    credl = new QLabel("Entrer le crédit :");
    debl = new QLabel("Entrer le débit :");
    compte = new QComboBox;
    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> compte_classiques=cm.get_liste_all_comptes();
    for(auto it=compte_classiques.begin();it!=compte_classiques.end(); it++)
    {
        QString line=(*it)->getNom();
        compte->addItem(line);
    }
    cred = new QDoubleSpinBox();
    deb = new QDoubleSpinBox();
    //compte->setText("");
    deb->setRange(0,10000000000000);
    deb->setValue(0);
    cred->setRange(0,10000000000000);
    cred->setValue(0);

    debit_layout = new QHBoxLayout();
    credit_layout = new QHBoxLayout();
    compte_layout = new QHBoxLayout();

    valider_triplet_layout = new QHBoxLayout();

    valider_triplet = new QPushButton("Valider le triplet");
    valider_triplet_layout->addWidget(valider_triplet);



    compte_layout->addWidget(comptel);
    compte_layout->addWidget(compte);
    debit_layout->addWidget(debl);
    debit_layout->addWidget(deb);
    credit_layout->addWidget(credl);
    credit_layout->addWidget(cred);

    couche->addLayout(quest_ref_layout);
    couche->addLayout(liste_transac);
    couche->addWidget(rapprocher_all);
    couche->addLayout(compte_layout);
    couche->addLayout(debit_layout);
    couche->addLayout(credit_layout);
    couche->addLayout(valider_triplet_layout);

    compte->setFixedWidth(350);
    deb->setFixedWidth(350);
    cred->setFixedWidth(350);
    setLayout(couche);

    QObject::connect(valider_triplet,SIGNAL(clicked()),this,SLOT(valider_rap_triplet()));


    State *pt2= new Non_Rapproche;
    qDebug()<<"nbr="<<vueTransactions->rowCount()<<"\n";
    int j=0;
    transactionManager &tm=transactionManager::getManager();
    list<transaction *> liste_transaction=tm.get_tab_transactions();
    for(auto it=liste_transaction.begin();it!=liste_transaction.end();it++) // Liste de toutes les transactions
    {
        if(typeid(*(*it)->get_current_state()).name() == typeid((*pt2)).name()){
            int k=0;
            int present=0;
            for(auto &it_:(*it)->get_tab_triplet()){
                if ((it_).get_compte().getNom()==nom_compte)
                {
                    present=1;
                    break;
                }

            }
            qDebug()<<"Present:"<<present<<endl;
            //HHeaderList<<"Date"<<"Reference"<<"Titre"<<"Comptes"<<"Débit"<<"Crédit";
            if(present==1){
                ref_trans->addItem(QString::number((*it)->get_reference()));
                for(auto &it_:(*it)->get_tab_triplet())
                {

                    vueTransactions->insertRow(0);
                    cout << 3;
                    qDebug()<<"Reference:"<<(*it)->get_reference()<<"\n";
                    j++;

                    QDate s1;
                    QString s2,d1;
                    QString ref;
                    ref.setNum((*it)->get_reference());
                    s1=(*it)->get_date();
                    d1=s1.toString();
                    qDebug()<<"Date:"<<s1<<"\n";
                    vueTransactions->setItem(0, 0, new QTableWidgetItem(d1));
                    vueTransactions->setItem(0, 1, new QTableWidgetItem(ref));
                    s2=(*it)->get_titre();
                    qDebug()<<"Titre:"<<s2<<"\n";
                    vueTransactions->setItem(0, 2, new QTableWidgetItem(s2));
                    QString nom;
                    nom=(it_).get_compte().getNom();
                    vueTransactions->setItem(0, 3, new QTableWidgetItem(nom));
                    qDebug()<<"Nom:"<<nom<<"\n";
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
                    k++;
                }

                vueTransactions->horizontalHeader()->stretchLastSection();
                vueTransactions->verticalHeader()->stretchLastSection();
            }
        }

    }
    // cacher les lignes restantes
    for(; j<vueTransactions->rowCount(); j++) vueTransactions->hideRow(j);
}

/// \fn Rapprochement::valider_rap_triplet
/// \brief Vérifier qu'une transaction possède bien la référence choisie et demander à l'utilisateur s'il veut ajouter un autre triplet.

void Rapprochement::valider_rap_triplet(){

    //ref = ref_trans->text().toInt();
    CompteManager & cm = CompteManager::getManager();
    c = dynamic_cast<Compte*>(cm.getCompte(compte->currentText()));
    ref = ref_trans->currentText().toInt();
    transactionManager &tm=transactionManager::getManager();
    int trouve = 0;
    for(auto &it : tm.get_tab_transactions()){
        if(it->get_reference()==ref_trans->currentText().toInt())
        {
            qDebug() << "Annulation de l'ancienne transaction...";
            trouve = 1;
            const QDate date= it->get_date();
            const QString titre = it->get_titre();
            const int ref= it->get_reference();

            transac_rapp = new transaction(date,ref,titre);
            transac_rapp->add_triplet(*c, cred->value(),  deb->value());
            deja_utilises.push_back(c);
        }
    }
    qDebug() << "Nouvelle transaction créée";
    if(trouve == 0){
        QMessageBox msgBox;
        msgBox.setText("Pas de transaction ayant cette référence.");
        msgBox.exec();
        close();
    }

    delete quest_ref_layout;
    delete question2;
    delete ref_trans;
    delete comptel;
    delete debl;
    delete credl;
    delete compte;
    delete deb;
    delete cred;
    delete debit_layout;
    delete credit_layout;
    delete compte_layout;
    delete valider_triplet;
    delete valider_triplet_layout;
    delete rapprocher_all;
    delete transfertsl;
    delete vueTransactions;
    delete liste_transac;

    setWindowTitle(QString("Demande ajout de triplet"));
    question_triplet_layout = new QHBoxLayout();
    question_triplet = new QLabel("Voulez-vous ajouter un triplet ?");
    question_triplet->setAlignment(Qt::AlignCenter);
    question_triplet_layout->addWidget(question_triplet);

    oui_triplet = new QPushButton("Oui");
    non_triplet = new QPushButton("Non");
    triplet_oui_non_layout = new QHBoxLayout;

    triplet_oui_non_layout->addWidget(non_triplet);
    triplet_oui_non_layout->addWidget(oui_triplet);

    couche->addLayout(question_triplet_layout);
    couche->addLayout(triplet_oui_non_layout);
    setLayout(couche);

    QObject::connect(oui_triplet,SIGNAL(clicked()),this,SLOT(adding_triplet()));
    QObject::connect(non_triplet,SIGNAL(clicked()),this,SLOT(valider_non_rapproche()));

}

/// \fn Rapprochement::adding_triplet
/// \brief Ajout d'un nouveau triplet dans la transaction.

void Rapprochement::adding_triplet(){
    qDebug() << "adding_triplet...";
    delete question_triplet_layout;
    delete question_triplet;
    delete triplet_oui_non_layout;
    delete oui_triplet;
    delete non_triplet;

    setWindowTitle(QString("Ajout d'un nouveau triplet"));

    comptel = new QLabel("Ecrire le nom du compte :");
    credl = new QLabel("Entrer le crédit :");
    debl = new QLabel("Entrer le débit :");
    compte = new QComboBox();
    compte = new QComboBox;
    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> compte_classiques=cm.get_liste_all_comptes();
    for(auto it=compte_classiques.begin();it!=compte_classiques.end(); it++)
    {
        bool trouve = false;
        for(auto & it2 : deja_utilises){
            if((*it)->getNom() == it2->getNom())
                trouve = true;
        }
        if(trouve == false){
            QString line=(*it)->getNom();
            compte->addItem(line);
        }
    }
    cred = new QDoubleSpinBox();
    deb = new QDoubleSpinBox();
    //compte->setText("");
    deb->setRange(0,10000000000000);
    deb->setValue(0);
    cred->setRange(0,10000000000000);
    cred->setValue(0);

    debit_layout = new QHBoxLayout();
    credit_layout = new QHBoxLayout();
    compte_layout = new QHBoxLayout();

    valider_triplet_layout = new QHBoxLayout();
    valider_triplet = new QPushButton("Valider le triplet");
    valider_triplet_layout->addWidget(valider_triplet);

    compte_layout->addWidget(comptel);
    compte_layout->addWidget(compte);
    debit_layout->addWidget(debl);
    debit_layout->addWidget(deb);
    credit_layout->addWidget(credl);
    credit_layout->addWidget(cred);

    couche->addLayout(compte_layout);
    couche->addLayout(debit_layout);
    couche->addLayout(credit_layout);
    couche->addLayout(valider_triplet_layout);

    compte->setFixedWidth(350);
    deb->setFixedWidth(350);
    cred->setFixedWidth(350);
    setLayout(couche);

    QObject::connect(valider_triplet,SIGNAL(clicked()),this,SLOT(demande_ajout_triplet()));
}

/// \fn Rapprochement::demande_ajout_triplet
/// \brief Vérifier l'existence du compte pour lequel est ajouté le triplet et demander n fois si l'utilisateur veut continuer à ajouter un triplet.

void Rapprochement::demande_ajout_triplet(){
    qDebug("Demande ajout triplet");
    int trouve_compte = 0;
    Compte_racine * cr = nullptr;
    CompteManager & cm = CompteManager::getManager();
    cr = cm.getCompte(compte->currentText());
    if(cr == nullptr)
    {
        QMessageBox msgBox;
        msgBox.setText("Ce compte n'existe pas.");
        msgBox.exec();
        close();
    }
    Compte & c = dynamic_cast<Compte &>(*cr);

    qDebug() << "OK compte trouvé";
    qDebug() << "débit:" << deb->value() << "crédit:" << cred->value();
    transac_rapp->add_triplet(c,  cred->value(), deb->value());
    deja_utilises.push_back(&c);
    delete comptel;
    delete credl;
    delete debl;
    delete compte;
    delete cred;
    delete deb;
    delete debit_layout;
    delete credit_layout;
    delete compte_layout;
    delete valider_triplet_layout;
    delete valider_triplet;

    setWindowTitle(QString("Demande ajout de triplet"));
    question_triplet_layout = new QHBoxLayout();
    question_triplet = new QLabel("Voulez-vous ajouter un triplet ?");
    question_triplet->setAlignment(Qt::AlignCenter);
    question_triplet_layout->addWidget(question_triplet);

    oui_triplet = new QPushButton("Oui");
    non_triplet = new QPushButton("Non");
    triplet_oui_non_layout = new QHBoxLayout;

    triplet_oui_non_layout->addWidget(non_triplet);
    triplet_oui_non_layout->addWidget(oui_triplet);

    couche->addLayout(question_triplet_layout);
    couche->addLayout(triplet_oui_non_layout);
    setLayout(couche);

    QObject::connect(oui_triplet,SIGNAL(clicked()),this,SLOT(adding_triplet()));
    QObject::connect(non_triplet,SIGNAL(clicked()),this,SLOT(valider_non_rapproche()));

}

/// \fn Rapprochement::valider_non_rapproche
/// \brief Vérifier la conformité de la nouvelle transaction.
/// Si elle est valide, l'ancienne transaction est supprimée et la nouvelle la remplace.
/// Si elle n'est pas valide, une erreur est renvoyée et le rapprochement bancaire est annulé.


void Rapprochement::valider_non_rapproche(){
    delete question_triplet_layout;
    delete question_triplet;
    delete triplet_oui_non_layout;
    delete oui_triplet;
    delete non_triplet;

    transactionManager &tm=transactionManager::getManager();
    int valeur_retour_transac =tm.ajouterTransaction(*transac_rapp);
    if(valeur_retour_transac == 0)
    {

        for(auto &it : tm.get_tab_transactions()){
            if(it->get_reference()== ref)
            {
                tm.annuler_transaction(*it);
                break;
            }
        }
    }
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
    qDebug() << "Transaction ajoutée";


    QMessageBox msgBox;
    msgBox.setText("L'opération est terminée.");
    msgBox.exec();
    mw->MAJ_listeComptes();
    setNon_rappV2();

}

/// \fn Transaction_viewer::refresh
/// \brief Mettre à jour la liste des transactions non rapprochées

void Rapprochement::refresh(){
    State *pt2= new Non_Rapproche;
    qDebug()<<"nbr="<<vueTransactions->rowCount()<<"\n";
    int j=0;
    transactionManager &tm=transactionManager::getManager();
    list<transaction *> liste_transaction=tm.get_tab_transactions();
    for(auto it=liste_transaction.begin();it!=liste_transaction.end();it++) // Liste de toutes les transactions
    {
        if(typeid(*(*it)->get_current_state()).name() == typeid((*pt2)).name()){
            int k=0;
            int present=0;
            for(auto &it_:(*it)->get_tab_triplet()){
                if ((it_).get_compte().getNom()==nom_compte)
                {
                    present=1;
                    break;
                }

            }
            qDebug()<<"Present:"<<present<<endl;
            //HHeaderList<<"Date"<<"Reference"<<"Titre"<<"Comptes"<<"Débit"<<"Crédit";
            if(present==1){
                ref_trans->clear();
                ref_trans->addItem(QString::number((*it)->get_reference()));
                for(auto &it_:(*it)->get_tab_triplet())
                {
                    vueTransactions->insertRow(0);
                    cout << 3;
                    qDebug()<<"Reference:"<<(*it)->get_reference()<<"\n";
                    j++;

                    QDate s1;
                    QString s2,d1;
                    QString ref;
                    ref.setNum((*it)->get_reference());
                    s1=(*it)->get_date();
                    d1=s1.toString();
                    qDebug()<<"Date:"<<s1<<"\n";
                    vueTransactions->setItem(0, 0, new QTableWidgetItem(d1));
                    vueTransactions->setItem(0, 1, new QTableWidgetItem(ref));
                    s2=(*it)->get_titre();
                    qDebug()<<"Titre:"<<s2<<"\n";
                    vueTransactions->setItem(0, 2, new QTableWidgetItem(s2));
                    QString nom;
                    nom=(it_).get_compte().getNom();
                    vueTransactions->setItem(0, 3, new QTableWidgetItem(nom));
                    qDebug()<<"Nom:"<<nom<<"\n";
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
                    k++;
                }

                vueTransactions->horizontalHeader()->stretchLastSection();
                vueTransactions->verticalHeader()->stretchLastSection();
            }
        }

    }
    // cacher les lignes restantes
    for(; j<vueTransactions->rowCount(); j++) vueTransactions->hideRow(j);
}

/// \fn Rapprochement::~Rapprochement()
/// \brief Destruction de l'objet Rapprochement. Tous les rapprochements sont écrits dans le fichier xml.

Rapprochement::~Rapprochement(){
    FichierXML & XML = FichierXML::getFichierXML();
    QString filename_compte = XML.getFichierCompte();
    //! Rendre un fichier vierge
    QFile file(filename_compte);
    file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    file.close();
    QString filename_transaction = XML.getFichierTransaction();
    QFile file2(filename_transaction);
    file2.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    file2.close();
    mw->writeXmlFile_comptes(filename_compte);
    mw->writeXmlFile_transactions(filename_transaction);
    deja_utilises.clear();
    list<Compte*>().swap(deja_utilises); //swap with an empty list
}
