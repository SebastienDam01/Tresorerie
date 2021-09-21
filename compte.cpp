#include "compte.h"
#include "transaction.h"


///\file compte.cpp
///\brief Fichiers de fonctions pour la classe Compte_racine, CompteVirtuel, Compte, CompteManager


/// \fn Compte_racine::operator==
/// \brief Permet de tester l'égalité entre deux comptes racines seulement en fonction de l'égalité de deux attributs: numéro et nom de comptes
/// \param c1, référence sur un compte_racine.
/// \return un booleen

bool Compte_racine::operator==(Compte_racine& c1){
    if(this->numero == c1.getNumero() && this->nom == c1.getNom()){
        return true;
    }
    return false;
}

/// \fn Compte::operator==
/// \brief Permet de tester l'égalité entre deux comptes classiques seulement en fonction de l'égalité de deux attributs: numéro et nom de comptes
/// \param c1, référence sur un compte classique.
/// \return un booleen

bool Compte::operator==(Compte_racine* c1){
    if(this->numero == c1->getNumero() && this->nom == c1->getNom()){
        return true;
    }
    return false;
}

/// \fn Compte::crediter
/// \brief Permet de créditer le compte d'un certain montant en fonction de son type
/// \param m le montant
/// \return Void


void Compte::crediter(double m){
    if(this->type==Actifs || this->type==Depenses){
        this->solde-=m;
        Compte_racine * aux = getParent();
        while (aux != nullptr) {
            aux->setSolde(aux->getSolde()-m);
            aux = aux->getParent();
        }
    }
    else{
        this->solde+=m;
        Compte_racine * aux = getParent();
        while (aux != nullptr) {
            aux->setSolde(aux->getSolde()+m);
            aux = aux->getParent();
        }
    }
}


/// \fn Compte::debiter
/// \brief Permet de débiter le compte d'un certain montant en fonction de son type
/// \param m le montant
/// \return Void

void Compte::debiter(double m){
    if(this->type==Actifs || this->type==Depenses){
        this->solde+=m;
        Compte_racine * aux = getParent();
        while (aux != nullptr) {
            aux->setSolde(aux->getSolde()+m);
            aux = aux->getParent();
        }
    }
    else{
        this->solde-=m;
        Compte_racine * aux = getParent();
        while (aux != nullptr) {
            aux->setSolde(aux->getSolde()-m);
            aux = aux->getParent();
        }
    }
}


/// \fn Compte_racine::Compte_racine
/// \brief Constructeur de Compte_racine
/// \param n le nom du compte
/// \param t le type du compte
/// \param s le solde initial
/// \return une instance de Compte_racine

Compte_racine::Compte_racine(const QString& n,type_compte t, const double s):solde(s),nom(n),type(t),parent_(nullptr)
{
    CompteManager & cm = CompteManager::getManager();
    numero = QString::number((cm.getNumeroCourant()));
}


/// \fn CompteVirtuel::CompteVirtuel
/// \brief Constructeur de CompteVirtuel
/// On a le DP factory method, qui clone l'objet passé en paramètre au moment de la construction d'un compte classique ou d'un compte virtuel.
/// Donc en créant un objet, on le clone et on le met dans la liste des comptes du CM
/// Pour faire référence à cet objet, on le recherchera dans la liste correspondante du CompteManager
/// \param n le nom du compte
/// \param t le type du compte
/// \param s le solde initial, toujours initialisté à 0 pour un virtuel
/// \return une instance de CompteVirtuel

CompteVirtuel::CompteVirtuel(const QString& n, type_compte t):Compte_racine(n,t,0){
    //si c'est virtuel on initialise à 0, les fils se chargeront de calculé le solde
    CompteManager& cm = CompteManager::getManager();
    cm.get_liste_comptes_virtuels().push_back(this->cloner());
    cm.incNumeroCourant();
}

/// \fn Compte::Compte
/// \brief Constructeur de Compte
/// On a le DP factory method, qui clone l'objet passé en paramètre au moment de la construction d'un compte classique ou d'un compte virtuel.
/// Donc en créant un objet, on le clone et on le met dans la liste des comptes du CM
/// Pour faire référence à cet objet, on le recherchera dans la liste correspondante du CompteManager
/// \param n le nom du compte
/// \param t le type du compte
/// \param s le solde initial
/// \return une instance de Compte

Compte::Compte(const QString& n,const type_compte t,const double s):Compte_racine(n,t,s){
    CompteManager& cm = CompteManager::getManager();
    cm.get_liste_all_comptes().push_back(this->cloner());
    cm.incNumeroCourant();
    std::cout<<"ok";
}

// ###### COMPTE VIRTUEL ####### //


/// \fn CompteVirtuel::Add
/// \brief Ajoute un Compte_racine dans la liste des enfants d'un compte virtuel
/// \param component le Compte_racine à ajouter

void CompteVirtuel::Add(Compte_racine * component)//pour ajouter un compte la liste des fils d'un compte virtuel
{
    this->children_.push_back(component);
    solde += component->getSolde();//on ajoute le solde du fils au compte virtuel
    Compte_racine* aux = getParent();
    while(aux != nullptr)//on ajoute le solde aux pères s'il yen a
    {
        aux->setSolde(aux->getSolde() + component->getSolde());
        cout<<"Solde = "<<aux->getSolde()<<endl;
        aux = aux->getParent();
    }
    component->SetParent(this);
}


/// \fn CompteVirtuel::RemoveChild
/// \brief Permet de retirer un Compte_racine de la liste des enfants du CompteVirtuel
/// \param component le Compte_racine à retirer

void CompteVirtuel::RemoveChild(Compte_racine& component)
{
    //enlever un compte de la liste des enfants d'un compte virtuel
    Compte_racine* enlever = CompteManager::getManager().getAllCompte(component.getNom());
    children_.remove(enlever);
}

/// \fn Compte::cloner
/// \brief Méthode cloner sur laquelle repose le DP Factory Method, on renvoie un clone de l'objet sur lequel on appelle cloner
/// \return Compte* un pointeur sur un Compte

Compte* Compte::cloner(){
    Compte* nouveau = new Compte(*this);
    return nouveau ;
}


/// \fn CompteVirtuel::cloner
/// \brief Méthode cloner sur laquelle repose le DP Factory Method, on renvoie un clone de l'objet sur lequel on appelle cloner
/// \return CompteVirtuel* un pointeur sur un CompteVirtuel

CompteVirtuel* CompteVirtuel::cloner(){
    CompteVirtuel* nouveau = new CompteVirtuel(*this);
    return nouveau ;
}

//initialisation de CM et numéro courant
CompteManager::handler CompteManager::cm_handler = CompteManager::handler();
//double CompteManager::numeroCourant = 1 ;   //commencer avec le premier compte qui a numéro 1

/// \fn CompteManager::getManager
/// \brief Récupérer l'instance unique de CompteManager à n'importe quel endroit du code via cette méthode statique
/// \return CompteManager& une référence sur l'instance unique de CompteManager

CompteManager& CompteManager::getManager()
{
    if(cm_handler.instance == nullptr)
    {
        cm_handler.instance= new CompteManager;
    }
    std::cout<<"cm créé"<<std::endl;
    return *(cm_handler.instance);
}

/// \fn CompteManager::libererManager
/// \brief Libération de la mémoire réservée à l'instance de CompteManager et destruction de l'objet

void CompteManager::libererManager()
{
    delete cm_handler.instance;
    cm_handler.instance = nullptr;
}

/// \fn CompteManager::~CompteManager()
/// \brief Destruction de l'objet CompteManager
/// Dans le destructeur, on détruit aussi tous les comptes contenus par le compte manager.
/// Pour ce faire, les listes sont vidées en détruisants objet par objet, qui sont des clones.
/// Les Compte stockés dans les CompteVirtuel seront détruits avant.

CompteManager::~CompteManager(){
    for(auto & it : all_comptes)
    {
        delete it;
    }
    for(auto & it : comptes_virtuels)
    {
        delete it;
    }
}

/// \fn CompteManager::already_exists
/// \brief Recherche d'un compte préexistant dans les listes du manager en fonction du nom passé en paramètre
/// \param nom_compte, le nom recherché
/// \return true si le compte existe, false sinon

bool CompteManager::already_exists(const QString& nom_compte){
    bool val = false;
    for(auto & it : comptes_virtuels){
        if(it->getNom() == nom_compte){
            val = true;
            return val;
        }
    }
    for(auto & it : all_comptes){
        if(it->getNom() == nom_compte){
            val = true;
            return val;
        }
    }
    return val;
}


/// \fn CompteManager::getCompteVirtuel
/// \brief Recherche d'un CompteVirtuel dans la liste adéquate du manager
/// \param nom_recherche le nom du compte virtuel recherché
/// \return Compte_racine* un pointeur sur l'objet recherché, nullptr s'il n'existe pas

Compte_racine* CompteManager::getCompteVirtuel(const QString & nom_recherche){ //compte virtuels
    for(auto & it : comptes_virtuels)
    {
        if(it->getNom() == nom_recherche)
        {
            return it;
        }
    }
    return nullptr;
}

/// \fn CompteManager::getCompte
/// \brief Recherche d'un Compte dans la liste adéquate du manager
/// \param nom_recherche le nom du compte recherché
/// \return Compte_racine* un pointeur sur l'objet recherché, nullptr s'il n'existe pas


Compte_racine* CompteManager::getCompte(const QString &nom_recherche){  //comptes classiques
    for(auto & it : all_comptes){
        if(it->getNom() == nom_recherche){
            return it;
        }
    }
    return nullptr;
}


/// \fn CompteManager::getAllCompte
/// \brief Recherche d'un compte (peu importe son type, virtuel ou classique) dans les deux listes du manager
/// \param nom_recherche le nom du compte  recherché
/// \return Compte_racine* un pointeur sur l'objet recherché, nullptr s'il n'existe pas

Compte_racine* CompteManager::getAllCompte(const QString& nom_recherche){  //tous les comptes
    for(auto & it : comptes_virtuels){
        if(it->getNom() == nom_recherche){
            return it;
        }
    }
    for(auto & ite : all_comptes){
        if(ite->getNom() == nom_recherche){
            return ite;
        }
    }
    return nullptr;
}


/// \fn CompteManager::Remove
/// \brief Méthode qui enlève le compte passé en paramètre de la liste adéquate du manager
/// Ce compte ne doit pas contenir de transactions s'il est de type Compte sinon il ne doit pas contenir d'autres comptes s'il est de type CompteVirtuel
/// \param component le compte à supprimer
/// \return -1 si CompteVirtuel qui a des fils (échec), -2 si Compte a des transactions (échec), 0 si succès

int CompteManager::Remove(Compte_racine * component)
{
    if(component->IsVirtuel()){ //supression d'un compte virtuel
        CompteVirtuel * compte_v = dynamic_cast<CompteVirtuel*>(component); //recup_cildren est une methode de compte virtuel seulement
        if(compte_v->recup_children().size() == 0){ //Le compte virtuel n'a pas de fils, on peut le supp
            if(component->getParent() != nullptr){ //On supprime le fils dans la liste des fils du pere
                CompteVirtuel * parent = dynamic_cast<CompteVirtuel*>(component->getParent());
                parent->RemoveChild(*component);
            }
            comptes_virtuels.remove(component);
        }
        else{
            return -1;
        }
    }
    else{  //Compte classique
        bool haveTransaction = false;
        transactionManager& tm = transactionManager::getManager();
        for(auto& it : tm.get_tab_transactions()){ //Acces aux transaction
            for(auto& it2 : (*it).get_tab_triplet()){ //Acces aux triplets pr chaque transaction
                if(it2.get_compte() == component){
                    haveTransaction = true;
                    break;
                }
            }
        }
        if(haveTransaction == false){
            if(component->getParent()){
                CompteVirtuel * parent = dynamic_cast<CompteVirtuel*>(component->getParent());
                parent->RemoveChild(*component);

            }
            all_comptes.remove(component);
        }
        else{
            return -2;
        }
    }
    return 0;

}


/// \fn CompteManager::load_comptes
/// \brief Charger les comptes présents dans le fichier xml dans les listes du CompteManager
/// \param f le nom du fichier xml

void CompteManager::load_comptes(const QString& f) {
    get_liste_comptes_virtuels().clear();

    //Création du fichier à partir du nom fourni
    QFile file(f);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"erreur ouverture lecture fichier compte";
    }

    // création du document et parsing du fichier xml
    QDomDocument xmlDoc;
    xmlDoc.setContent(&file); // on met le contenu du fichier dans xmlDoc



    /* Traitement des comptes virtuels */
    QDomNodeList comptes_2 = xmlDoc.elementsByTagName("Comptes_virtuels");
    // identifie toutes les balises où il y a comptes virtuels au début

    for (int i = 0; i < comptes_2.size(); i++)
    {
        QDomElement cptnode = comptes_2.item(i).toElement();

        QString numero = cptnode.attribute("numero", "");
        QString nom = cptnode.attribute("nom", "");
        QString name_parent = cptnode.attribute("parent", ""); // on recupere le nom du parent
        Compte_racine * parent_compte=getCompteVirtuel(name_parent); // puis on assigne le compte parent correspondant
        QString str_type = cptnode.attribute("type", "");
        type_compte type;
        if (str_type == "Passifs")
            type = type_compte::Passifs;
        else if (str_type == "Actifs")
            type = type_compte::Actifs;
        else if (str_type == "Depenses")
            type = type_compte::Depenses;
        else if (str_type == "Recettes")
            type = type_compte::Recettes;

        CompteVirtuel compte_v(nom,type);
        Compte_racine* clone = getCompteVirtuel(compte_v.getNom());
        // compte virtuel mis par défaut dans la liste via le constructeur
        clone->parent_ = parent_compte;

        if(parent_compte != nullptr)
        {
            //cast car on ajoute des comptes racines dans la liste, pas possible de discriminer mtn
            CompteVirtuel * parent_Virtuel = dynamic_cast<CompteVirtuel *>(parent_compte);
            parent_Virtuel->Add(clone);
        }
    }
    /* Traitement des comptes classiques */
    QDomNodeList comptes = xmlDoc.elementsByTagName("Comptes_classiques");

    // on crée autant de comptes qu'il y a d'éléments dans le document
    // identifie toutes les balises où il y a compte
    get_liste_all_comptes().clear();
    for (int i = 0; i < comptes.size(); i++)
    {
        QDomElement cptnode = comptes.item(i).toElement();
        QString numero = cptnode.attribute("numero", "");
        //double solde = cptnode.attribute("solde", "").toDouble();
        QString nom = cptnode.attribute("nom", "");
        QString name_parent = cptnode.attribute("parent", ""); // on recupere le nom du parent
        // qDebug()<<"Nom_compte"<<nom;
        // qDebug()<<"Nom_parent:"<<name_parent<<endl;
        Compte_racine * parent_compte=getCompteVirtuel(name_parent); // puis on assigne le compte parent correspondant

        QString str_type = cptnode.attribute("type", "");
        type_compte type;
        if (str_type == "Passifs")
            type = type_compte::Passifs;
        else if (str_type == "Actifs")
            type = type_compte::Actifs;
        else if (str_type == "Depenses")
            type = type_compte::Depenses;
        else if (str_type == "Recettes")
            type = type_compte::Recettes;
        QString date = cptnode.attribute("date_dernier_rapprochement", ""); // on recupere le nom du parent
        qDebug() << "Date lue dernier rapprochement" << date;
        QDate date_= QDate::fromString(date, "dd/MM/yyyy");
        qDebug() << "Date convertie en QDate à partir de la lecture" <<date_;
        Compte compte(nom,type);
        compte.setDateDernierRapprochement(date_);
        Compte_racine* clone = getCompte(compte.getNom());
        clone->setDateDernierRapprochement(date_);
        clone->parent_ = parent_compte;
        if(parent_compte!=nullptr)
        {
            CompteVirtuel * parent_Virtuel = dynamic_cast<CompteVirtuel *>(parent_compte);
            parent_Virtuel->Add(clone);
        }
    }

    file.close();
}
