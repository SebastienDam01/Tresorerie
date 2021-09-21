#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "choix_type.h"

/// \file mainwindow.cpp
/// \brief Fichiers de fonctions pour la classe mainwindow

std::ostream& operator<<(std::ostream& stream, const QString& qstr)
{
    stream << qstr.toStdString();
    return stream;
}

/// \fn MainWindow::MainWindow
/// \brief Constructeur de la MainWindow
/// \param transfertV un pointeur sur le widget Transaction_viewer
/// \param parent le widget parent
/// \return Une instance de MainWindow

MainWindow::MainWindow(Transaction_viewer * transfertV, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    v(transfertV)
{
    ui->setupUi(this);
    ui->Liste_Operation->addItem("Cloture");
    ui->Liste_Operation->addItem("Mon bilan");
    ui->Liste_Operation->addItem("Mon compte de resultat");
    ui->Liste_Operation->addItem("Historique des transactions");
    ui->Liste_Operation->addItem("");
    ui->Liste_Operation->addItem("Reinitialiser l'application");
    QObject::connect(ui->Liste_Operation, SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(choix_affichage(QListWidgetItem*)));
    MAJ_listeComptes();
}


/// \fn MainWindow::addRoot
/// \brief Permet d'ajouter un compte dans le QTreeWidget en charge d'afficher l'arborescence des comptes
/// On veut ajouter un CompteVirtuel à l'arborescence (en tant que racine ou à son père) puis ajouter ses fils en dessous par appel à AddEnfant
/// \param c le Compte_racine à ajouter dans l'arborescence
/// \param parent le QTreeWidget parent

void MainWindow::addRoot(Compte_racine *c, QTreeWidgetItem * parent)// = nullptr au debut puis le pere
{
    QTreeWidgetItem * item;
    if(c->getParent() == nullptr) // si le compte n'a pas de pere, on lie le Qtreewidgetitem directement à treeWidget
    {
        item = new QTreeWidgetItem(ui->treeWidget);
    }
    else
    {
        item = new QTreeWidgetItem();
    }
    QString nom = c->getNom() + QString(" [Virtuel]");
    item->setText(0,nom);
    std::ostringstream strs;
    strs << c->getSolde();
    std::string str = strs.str();
    item->setText(1,QString::fromStdString(str));
    //sur Passif, un virtuel qui n'a pas de père, on ajoute l'item en top level de l'arbre sinon on l'affiche dans la suite de l'arborescence
    if(c->getParent() == nullptr)
    {
        ui->treeWidget->addTopLevelItem(item);
    }
    else
        parent->addChild(item);

    //dynamic cast pour récupérer la méthode qui accède au tableau des enfants
    CompteVirtuel * CV = dynamic_cast<CompteVirtuel*>(c);
    for(auto & it_children : CV->recup_children())
    {
        parent = item;
        addEnfant(it_children, parent);
    }
}

/// \fn MainWindow::addEnfant
/// \brief Permet d'ajouter un Compte sous une racine
/// Ajouter les fils d'une racine en dessous dans l'arborescence, et recommencer le processus si jamais l'un des Enfants serait virtuel avec des fils, via un appel à AddRoot
/// \param c le Compte_racine à ajouter dans l'arborescence
/// \param parent le QTreeWidget parent

void MainWindow::addEnfant(Compte_racine *c, QTreeWidgetItem * parent)
{
    if(!c->IsVirtuel()){
        QTreeWidgetItem * child = new QTreeWidgetItem();
        child->setText(0,c->getNom());
        std::ostringstream strs;
        strs << c->getSolde();
        std::string str = strs.str();
        child->setText(1,QString::fromStdString(str));
        parent->addChild(child);
    }
    else{
        addRoot(c,parent);
    }
}

/// \fn MainWindow::MAJ_listeComptes
/// \brief Permet de mettre à jour l'arborescence des comptes après une modification dans les listes du manager (ajout, suppression, modification...)

void MainWindow::MAJ_listeComptes()
{
    ui->treeWidget->clear();
    ui->treeWidget->setColumnCount(2);
    QStringList *header = new QStringList();
    header->append("Racine");
    header->append("Solde");
    ui->treeWidget->setHeaderLabels(*header);
    ui->treeWidget->setColumnWidth(0,300);
    ui->treeWidget->setColumnWidth(1,100);
    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> l = cm.get_liste_comptes_virtuels();
    list<Compte_racine*> l2 = cm.get_liste_all_comptes();
    for (int i = 0; i<4;i++){
        for(auto& it : l)
        {
            //comptes virtuels sans père (Actif,Passif...)
            if(it->getParent() == nullptr && it->getType() == i)
                addRoot(it,nullptr);
        }
        for(auto& it : l2)
        {
            //comptes classiques sans pere (Resultat, Capitaux Propres …)
            if(it->getParent() == nullptr && it->getType() == i)
            {
                QTreeWidgetItem * item = new QTreeWidgetItem(ui->treeWidget);
                QString nom = it->getNom();
                item->setText(0,nom);
                std::ostringstream strs;
                strs << it->getSolde();
                std::string str = strs.str();
                item->setText(1,QString::fromStdString(str));
                ui->treeWidget->addTopLevelItem(item);
            }
        }
    }
}

/// \fn MainWindow::~MainWindow
/// \brief Destructeur de MainWindow
/// On sauvegarde le contexte en appelant diverses fonctions qui écrivent dans nos deux fichiers xml puis on détruit MainWindow


MainWindow::~MainWindow()
{

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
    writeXmlFile_comptes(filename_compte);
    writeXmlFile_transactions(filename_transaction);

    delete ui;

}

/// \fn MainWindow::choix_affichage
/// \brief Permet de choisir le widget à afficher en fonction de la section sur laquelle on a cliqué dans le menu des fonctionnalités.
/// \param item le widget sur lequel nous avons cliqué
/// \return affichage du widget correspondant à l'appel de la fonctionnalité cliquée par l'utilisateur.

void MainWindow::choix_affichage(QListWidgetItem* item)
{

    if(ui->Liste_Operation->item(0) == item)
    {
        //affiche mes transactions
        v->show();
        v->refresh(0);
    }
    if(ui->Liste_Operation->item(1) == item)
    {
        a= new all_rapprochements();
        a->show();
    }
    if(ui->Liste_Operation->item(2) == item){
        cloture(this);
        MAJ_listeComptes();
    }
    if(ui->Liste_Operation->item(3) == item){
        b = new bilan_viewer();
        b->setAttribute(Qt::WA_DeleteOnClose);
        b->show();
    }
    if(ui->Liste_Operation->item(4) == item){
        formulaire = new formulaire_date();
        formulaire->setAttribute(Qt::WA_DeleteOnClose);
        formulaire->show();
    }
    if(ui->Liste_Operation->item(5) == item){
        transac = new all_transactions();
        transac->setAttribute(Qt::WA_DeleteOnClose);
        transac->show();
    }
    if(ui->Liste_Operation->item(7) == item){
        reinitialiserXML();
        MAJ_listeComptes();
    }
    v->MAJ_liste_comptes();
}

/// \fn MainWindow::on_Ajouter_Compte_clicked
/// \brief Ajouter un compte dans l'arborescence sous le compte sur lequel on vient de cliquer ou sous la racine
/// La fonction est déclenchée lors d'un click sur le bouton ajouter

//Le QObject::connect est pris en charge par QTdesigner
void MainWindow::on_Ajouter_Compte_clicked() //Ajouter_Compte
{
    CompteManager& cm = CompteManager::getManager();
    bool cancelPressed;
    QString nom_new_compte = QInputDialog::getText(this, "Nouveau Compte", "Entrez le nom du nouveau compte :", QLineEdit::Normal, "", &cancelPressed);
    if(!cancelPressed){
        qDebug()<< "cancel";
    }
    else{

        if(cm.already_exists(nom_new_compte)){
            QMessageBox::warning(this, "Erreur", "Un compte du même nom existe déjà");
            return;
        }
        else if(nom_new_compte.contains("[") or nom_new_compte.contains("]")){
            QMessageBox::warning(this, "Erreur", "Caractère interdit dans le nom du compte");
            return;
        }
        if(ui->treeWidget->currentItem() == nullptr){ // Si le compte n'a pas de pere
            int virtuel = QMessageBox::question(this, "Nouveau Compte", "Ce compte est-il virtuel ?", QMessageBox ::Yes | QMessageBox::No | QMessageBox ::Cancel);
            if(virtuel == QMessageBox::Cancel){
                //Cancel automatiquement
            }
            else if (virtuel == QMessageBox::Yes){
                //Demande le type car n'a pas de père :
                choix_type ct;
                ct.exec();
                type_compte type = ct.choix();
                if(type == (type_compte)-1){
                    return;
                }
                //detruire puis envoyer l'info
                CompteVirtuel new_compte_v(nom_new_compte,type);
                //On ajoute à l'arborescence
                QTreeWidgetItem * item = new QTreeWidgetItem(ui->treeWidget);
                item->setText(0,nom_new_compte + QString(" [virtuel]"));
                QString valueAsString = QString::number(new_compte_v.getSolde());
                item->setText(1,valueAsString);
                ui->treeWidget->addTopLevelItem(item);
            }
            else{
                //Demande le type car n'a pas de père :
                choix_type ct;
                ct.exec();
                type_compte type = ct.choix();

                if(type == (type_compte)-1){
                    return;
                }
                Compte new_compte(nom_new_compte,type);
                Compte* clone_new_compte = dynamic_cast<Compte*>(cm.getCompte(new_compte.getNom()));
                if(type == Actifs || type == Passifs)//solde initial potentiel
                {
                    int HaveSolde = QMessageBox::question(this, "Nouveau Compte", "Le compte possède-il un solde initial", QMessageBox ::Yes | QMessageBox::No | QMessageBox::Cancel);
                    if(HaveSolde == QMessageBox::Cancel){
                        qDebug() << "Cancel";
                        return;
                        //Cancel automatiquement
                    }
                    else if(HaveSolde == QMessageBox::Yes){
                        capitaux = new capitaux_viewer(clone_new_compte);
                        capitaux->setCreateur(this);
                        //lorsque l'on ferme le widget capitaux, on le détruit
                        capitaux->setAttribute(Qt::WA_DeleteOnClose);
                        connect(capitaux,SIGNAL(destroyed(QObject*)),this,SLOT(MAJ_liste_comptes()));
                        connect(capitaux,SIGNAL(destroyed(QObject*)),v,SLOT(MAJ_liste_comptes()()));
                        capitaux->show();
                    }
                    else if(HaveSolde == QMessageBox::No){
                        qDebug() << "No";
                    }
                    else{
                        qDebug() << "Ni oui ni non";
                        return;
                    }
                }
            }
        }
        else{
            QString nom_parent = ui->treeWidget->currentItem()->text(0);
            // On recupere la taille et on resize de la taille de la chaine de caractères ' [virtuel]' soit 10 caractères.
            int size = nom_parent.size();
            nom_parent.resize(size-10);
            Compte_racine * parent = cm.getCompteVirtuel(nom_parent);
            if(parent == nullptr){
                QMessageBox::critical(this, "Erreur", "Impossible de créer un compte à partir d'un compte non-virtuel");
            }
            else{

                int virtuel = QMessageBox::question(this, "Nouveau Compte", "Ce compte est-il virtuel ?", QMessageBox ::Yes | QMessageBox::No | QMessageBox::Cancel);
                if(virtuel == QMessageBox::Cancel){
                    //Cancel automatiquement
                    return;
                }
                else if (virtuel == QMessageBox ::Yes){
                    CompteVirtuel new_compte_v(nom_new_compte, parent->getType());
                    CompteVirtuel* clone_new_compte_v = dynamic_cast<CompteVirtuel*>(cm.getCompteVirtuel(new_compte_v.getNom()));
                    CompteVirtuel* parent_virtuel = dynamic_cast<CompteVirtuel*>(parent);
                    parent_virtuel->Add(clone_new_compte_v);
                    //On ajoute à l'arborescence
                    QTreeWidgetItem * pere = ui->treeWidget->currentItem();
                    QTreeWidgetItem * item = new QTreeWidgetItem();
                    item->setText(0,nom_new_compte + QString(" [virtuel]"));
                    QString valueAsString = QString::number(new_compte_v.getSolde());
                    item->setText(1,valueAsString);
                    pere->addChild(item);
                }
                else{
                    Compte new_compte(nom_new_compte, parent->getType());
                    Compte* clone_new_compte = dynamic_cast<Compte*>(cm.getCompte(new_compte.getNom()));
                    CompteVirtuel* parent_virtuel = dynamic_cast<CompteVirtuel*>(parent);
                    parent_virtuel->Add(clone_new_compte);
                    if(parent->getType() == Actifs || parent->getType() == Passifs)//solde initial potentiel
                    {
                        int HaveSolde = QMessageBox::question(this, "Nouveau Compte", "Le compte possède-il un solde initial", QMessageBox ::Yes | QMessageBox::No | QMessageBox::Cancel);
                        if(HaveSolde == QMessageBox::Cancel){
                            return;
                        }
                        else if(HaveSolde == QMessageBox::Yes){
                            capitaux = new capitaux_viewer(clone_new_compte);
                            capitaux->setCreateur(this);
                            //lorsque l'on ferme le widget capitaux, on le détruit
                            capitaux->setAttribute(Qt::WA_DeleteOnClose);
                            connect(capitaux,SIGNAL(destroyed(QObject*)),this,SLOT(MAJ_liste_comptes()));
                            connect(capitaux,SIGNAL(destroyed(QObject*)),v,SLOT(MAJ_liste_comptes()()));
                            capitaux->show();
                        }
                        else{
                            qDebug() << "No";
                        }
                    }
                }

            }
            //! On pourrait tout détruire pour tout reconstruire (plus simple à écrire) mais beaucoup plus consommateur
        }
        MAJ_listeComptes();
        v->MAJ_liste_comptes();
    }
}

/// \fn MainWindow::writeXmlFile_comptes
/// \brief Assurer la persistence du système en écrivant les modifications apportées à la liste des comptes dans un fichier XML
/// \param filename le nom du fichier
void MainWindow::writeXmlFile_comptes(const QString& filename)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("Comptes");
    doc.appendChild(root);

    CompteManager& cm = CompteManager::getManager();
    list<Compte_racine*> comptes_classiques = cm.get_liste_all_comptes();
    qDebug() << "Taille liste classique dans le write :" << comptes_classiques.size();
    list<Compte_racine*> comptes_virtuels = cm.get_liste_comptes_virtuels();
    qDebug() << "Taille liste virtuelle dans le write :" << comptes_virtuels.size();
    // on va itérer sur tous les comptes à exporter
    // l'élément correspond au nom du tag dans la balise
    for (auto& it : comptes_classiques)
    {
        Compte *c=dynamic_cast<Compte *>(it);
        QDomElement cptElement = doc.createElement("Comptes_classiques");

        // on remarquera que l'ordre d'ajout des attributs n'est pas
        // nécessairement le même que l'ordre des lignes qui suivent
        cptElement.setAttribute("nom", (it->getNom()));
        cptElement.setAttribute("numero", (it->getNumero()));
        if(it->getParent()!=nullptr){
            cptElement.setAttribute("parent", (it->getParent()->getNom()));
        }
        else {
            cptElement.setAttribute("parent", "null");
        }
        cptElement.setAttribute("solde", QString::number(it->getSolde()));
        int type=it->getType();
        QString type_elem;
        if(type==0){
            type_elem="Actifs";
        }
        else if(type==1){
            type_elem="Passifs";
        }
        else if(type==2){
            type_elem="Depenses";
        }
        else if(type==3){
            type_elem="Recettes";
        }
        cptElement.setAttribute("type", type_elem);
        QDate date= c->getDateDernierRapprochement();
        qDebug() << "date type date"<< date;
        QString date_string=date.toString("dd/MM/yyyy");
        qDebug() << "date convertie dernier rapprochement" << date_string;
        cptElement.setAttribute("date_dernier_rapprochement", date_string);
        // on ajoute le compte à l'élément principal
        root.appendChild(cptElement);
    }
    for (auto& it : comptes_virtuels)
    {
        QDomElement cptElement_2 = doc.createElement("Comptes_virtuels");

        cptElement_2.setAttribute("nom", (it->getNom()));
        cptElement_2.setAttribute("numero", (it->getNumero()));
        if(it->getParent()!=nullptr){
            cptElement_2.setAttribute("parent", (it->getParent()->getNom()));
        } else {
            cptElement_2.setAttribute("parent", "null");
        }
        cptElement_2.setAttribute("solde", QString::number(it->getSolde()));
        int type=it->getType();
        QString type_elem;
        if(type==0){
            type_elem="Actifs";
        }
        else if(type==1){
            type_elem="Passifs";
        }
        else if(type==2){
            type_elem="Depenses";
        }
        else if(type==3){
            type_elem="Recettes";
        }
        cptElement_2.setAttribute("type", type_elem);

        // on ajoute le compte à l'élément principal
        root.appendChild(cptElement_2);
    }
    // ouverture du fichier en écriture
    QFile outfile(filename);
    if (!outfile.open(QIODevice::ReadWrite))
        return;
    // création d'un flux pour gérer la sortie
    QTextStream stream(&outfile);
    // export du document
    stream << doc.toString();
    // fermeture du fichier
    outfile.close();
}


/// \fn MainWindow::writeXmlFile_transactions
/// \brief Assurer la persistence du système en écrivant les modifications apportées à la liste des transaction dans un fichier XML
/// \param filename le nom du fichier

void MainWindow::writeXmlFile_transactions(const QString& filename)
{
    QDomDocument doc_transac;
    QDomElement root = doc_transac.createElement("Transactions");
    doc_transac.appendChild(root);
    list<transaction*> transactions = transactionManager::getManager().get_tab_transactions();
    // on va itérer sur toutes les transactions à exporter
    for (auto& it : transactions)
    {
        // l'élément correspond au nom du tag dans la balise
        QDomElement transac = doc_transac.createElement("Transaction_");

        // on remarquera que l'ordre d'ajout des attributs n'est pas
        // nécessairement le même que l'ordre des lignes qui suivent
        transac.setAttribute("reference", (it->get_reference()));
        QDate date= it->get_date();
        qDebug() << "date type date"<< date;
        QString date_string=date.toString("dd/MM/yyyy");
        qDebug() << "date convertie" << date_string;
        transac.setAttribute("date", date_string);
        transac.setAttribute("titre", (it->get_titre()));
        transac.setAttribute("state", (typeid(*(it->get_current_state())).name()));
        for(auto &it_:it->get_tab_triplet())
        {
            QDomElement triplet = doc_transac.createElement("Triplet");
            triplet.setAttribute("Nom_Compte", (it_.get_compte().getNom()));
            triplet.setAttribute("Credit", (it_.get_credit()));
            triplet.setAttribute("Debit",(it_.get_debit()));
            transac.appendChild(triplet);
        }
        root.appendChild(transac);
    }
    // ouverture du fichier en écriture
    QFile outfile(filename);
    if (!outfile.open(QIODevice::ReadWrite))
        return;
    // création d'un flux pour gérer la sortie
    QTextStream stream(&outfile);
    // export du document
    stream << doc_transac.toString();
    // fermeture du fichier
    outfile.close();
}


/// \fn MainWindow::on_Supprimer_Compte_clicked
/// \brief Permet de supprimer le compte sur lequel on vient de cliquer si les conditions sont ok

void MainWindow::on_Supprimer_Compte_clicked()
{
    CompteManager& cm = CompteManager::getManager();
    if(ui->treeWidget->currentItem() == nullptr){
        QMessageBox::warning(this, "Erreur", "Veuillez séléctionner un compte");
    }
    else{
        int conf = QMessageBox::question(this, "Supprimer", "Etes-vous sur de vouloir supprimer ce compte ?", QMessageBox ::Yes | QMessageBox::No);
        if(conf == QMessageBox::Yes){
            QString nom_compte_supp = ui->treeWidget->currentItem()->text(0);
            if(cm.getAllCompte(nom_compte_supp) != nullptr){
                int reussi = cm.Remove(cm.getAllCompte(nom_compte_supp));
                if(reussi == -2)
                    QMessageBox::warning(this, "Erreur", "Ce compte ne peut pas être supprimer car il contient des transactions");
                if(reussi == -1)
                    QMessageBox::warning(this, "Erreur", "Ce compte ne peut pas être supprimer car il contient d'autres comptes");
            }
            else{
                int size = nom_compte_supp.size();
                nom_compte_supp.resize(abs(size-10));
                int reussi = cm.Remove(cm.getAllCompte(nom_compte_supp));
                if(reussi == -2)
                    QMessageBox::warning(this, "Erreur", "Ce compte ne peut pas être supprimer car il contient des transactions");
                if(reussi == -1)
                    QMessageBox::warning(this, "Erreur", "Ce compte ne peut pas être supprimer car il contient d'autres comptes");
            }
            // On efface l'ardoise pour réafficher la nouvelle arborescence mise à jour
            MAJ_listeComptes();
            v->MAJ_liste_comptes();
        }
    }
}

/// \fn MainWindow::on_Rapprocher_Compte_clicked
/// \brief Lancer le rapprochement du compte sur lequel on vient de cliquer

// Le connect est réalisé via QT Designer qui a construit par défaut cette fonction lorsque l'on a spécifié dans le designer que c'était au signal clicked() qu'il devait s'enclencher
void MainWindow::on_Rapprocher_Compte_clicked()
{
    if(ui->treeWidget->currentItem() == nullptr){
        QMessageBox::warning(this, "Erreur", "Veuillez séléctionner un compte");
    }
    else{
        transactionManager &tm=transactionManager::getManager();
        QString nom_compte = ui->treeWidget->currentItem()->text(0);
        tm.rapprocher(nom_compte, this);
    }
}

/// \fn MainWindow::reinitialiserXML
/// \brief permet de remettre à 0 le système et les fichiers

void MainWindow::reinitialiserXML(){
    QString filename = ".export_transactions.xml";
    QString filename_2 = ".export_comptes.xml";
    //! Rendre un fichier vierge
    FILE *fp = fopen(".export_transactions.xml", "w");
    fclose(fp);
    FILE *fp2 = fopen(".export_comptes.xml", "w");
    fclose(fp2);
    CompteManager& cm = CompteManager::getManager();
    cm.destroy_listes();
    transactionManager & tm = transactionManager::getManager();
    tm.detroy_transac();
    /*! D'abord les comptes */
    QDomDocument doc;
    QDomElement root2 = doc.createElement("Comptes");
    doc.appendChild(root2);
    // ouverture du fichier en écriture
    QFile outfile2(filename_2);
    if (!outfile2.open(QIODevice::ReadWrite))
        return;
    // création d'un flux pour gérer la sortie
    //root2.appendChild();
    QTextStream stream2(&outfile2);
    // export du document
    stream2 << doc.toString();
    // fermeture du fichier
    outfile2.close();

    /*! Puis les transactions */
    QDomDocument doc_transac;
    QDomElement root = doc_transac.createElement("Transactions");
    doc_transac.appendChild(root);
    // ouverture du fichier en écriture
    QFile outfile(filename);
    if (!outfile.open(QIODevice::ReadWrite))
        return;
    // création d'un flux pour gérer la sortie
    QTextStream stream(&outfile);
    // export du document
    stream << doc_transac.toString();
    // fermeture du fichier
    outfile.close();
}

/// \fn MainWindow::on_Modifier_Compte_clicked
/// \brief Permet de modifier le nom du compte sur lequel on a cliqué si le nouveau nom désiré n'existe pas encore puis MAJ de l'aborescence

void MainWindow::on_Modifier_Compte_clicked()
{
    CompteManager& cm = CompteManager::getManager();
    if(ui->treeWidget->currentItem() == nullptr){
        QMessageBox::warning(this, "Erreur", "Veuillez séléctionner un compte");
    }
    else{
        bool cancelPressed;
        QString nouv_nom = QInputDialog::getText(this, "Modification", "Entrez le nouveau nom du compte :", QLineEdit::Normal, "", &cancelPressed);
        if(!cancelPressed){
            return;
        }
        if(cm.already_exists(nouv_nom)){
            QMessageBox::warning(this, "Erreur", "Un compte du même nom existe déjà");
        }
        else{
            Compte_racine * c = cm.getAllCompte(ui->treeWidget->currentItem()->text(0));
            if(c == nullptr){ //Cas des compte virtuel ou il faut retirer [Virtuel]
                QString old_nom = ui->treeWidget->currentItem()->text(0);
                int size = old_nom.size();
                old_nom.resize(size - 10);
                c = cm.getAllCompte(old_nom);
                if(c != nullptr){
                    c->setNom(nouv_nom);
                }
            }
            else
                c->setNom(nouv_nom);
            // On efface l'ardoise pour réafficher la nouvelle arborescence mise à jour
            MAJ_listeComptes();
        }
    }
}


/// \fn MainWindow::on_actionOuvrir_triggered
/// \brief Permet, à une nouvelle ouverture de nouveaux fichiers, de mettre à jour l'arborescense des comptes.

void MainWindow::on_actionOuvrir_triggered()
{
    change_fichiers_XML();
    MAJ_listeComptes();
}
