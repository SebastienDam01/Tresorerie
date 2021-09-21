#ifndef COMPTE_H_INCLUDED
#define COMPTE_H_INCLUDED

#include <algorithm>
#include <iostream>
#include <list>
#include <string>
#include <string.h>
#include <QString>
#include <QMessageBox>
#include <QDate>

using namespace std;

///\file compte.h
///\brief Fichier d'en-tête pour la gestion des comptes.

/// \union <type_compte>
/// \brief Définition des 4 types possibles pour un compte, conformément au cahier des charges.

enum type_compte {Actifs,Passifs,Depenses,Recettes};

class CompteManager ;

/// \class Compte_racine
/// \brief Classe abstraite compte racine qui permet de définir les méthodes pour l'héritage
/// Faite d'après le DP Composite et Factory Method (fonction cloner)

class Compte_racine
{
    friend class CompteManager ;

protected:

    //attributs
    Compte_racine *parent_;
    double solde;
    QString nom;
    QString numero;
    type_compte type;
    QDate date_dernier_rapprochement = QDate(0,0,0);

public:

    Compte_racine(const QString& n,type_compte t, const double s=0);
    /// \fn Compte_racine::~Compte_racine
    /// \brief Destructeur virtuel offrant des possibilités d'adaptation
    virtual ~Compte_racine() {}

    /// \fn Compte_racine::SetParent
    /// \brief Permet d'initialiser l'attribut parent avec le compte correspondant
    void SetParent(Compte_racine *parent){ parent_ = parent; }
    /// \fn Compte_racine::getSolde
    /// \brief Permet d'obtenir le solde du compte concerné
    double getSolde(){ return solde; }
    /// \fn Compte_racine::getNom
    /// \brief Permet d'obtenir le nom du compte concerné
    const QString & getNom() const { return nom; }
    /// \fn Compte_racine::getNumero
    /// \brief Permet d'obtenir le numero du compte concerné
    const QString & getNumero() const { return numero; }
    /// \fn Compte_racine::getParent
    /// \brief Permet d'obtenir le compte parent du compte concerné
    Compte_racine * getParent() const { return parent_; }
    /// \fn Compte_racine::getDateDernierRapprochement
    /// \brief Permet d'obtenir la date de dernier rapprochement du compte concerné
    const QDate & getDateDernierRapprochement()const{return date_dernier_rapprochement;}

    /// \fn Compte_racine::setDateDernierRapprochement
    /// \brief Permet de modifier la date de dernier rapprochement du compte concerné
    /// \param QDate d, la date d fournie pour modifier la date de dernier rapprochement
    void setDateDernierRapprochement(const QDate & d){date_dernier_rapprochement = d;}

    /// \fn Compte_racine::setNom
    /// \brief Permet de modifier le nom du compte concerné
    /// \param QString & n, une référence n fournie pour modifier le nom du compte
    void setNom(const QString & n){nom = n;}

    /// \fn Compte_racine::getType
    /// \brief Permet d'obtenir le type du compte concerné parmi les types possibles de l'énumération.
    type_compte getType() const { return type; }

    /// \fn Compte_racine::setSolde
    /// \brief Permet de modifier le solde du compte concerné
    /// \param Double s, un montant de type double qui sera associé à l'attribut solde du compte concerné
    void setSolde(double s){solde = s;}

    /// \fn Compte_racine::IsVirtuel
    /// \brief Indique si le compte concerné est virtuel ou non, cette méthode se base sur le polymorphisme.
    virtual bool IsVirtuel() const{ return false; }

    virtual Compte_racine* cloner() = 0;

    bool operator==(Compte_racine& c1);
};

/// \class Compte
/// \brief Classe qui définit un compte classique destiné à contenir des opérations bancaires, c'est-à-dire des transactions dans notre architecture
///
/// Utilisation du DP Factory method : méthode cloner
/// Hérite de Compte_racine
/// Contient : méthodes créditer() débiter() et opérateur d'égalité

class Compte : public Compte_racine
{
    friend class CompteManager ;
public:
    Compte(const QString& n,const type_compte t,const double s=0);

    void crediter(double m);
    void debiter(double m);

    Compte* cloner() override;
    bool operator==(Compte_racine* c1);
};

/// \class CompteVirtuel
/// \brief Classe Compte Virtuel qui peut stocker des Comptes et Comptes Virtuels.
/// Faite d'après le DP Composite et Factory Method
/// Elle permet de construire une arborescence de comptes classiques ou virtuels
/// Contient une liste chainées pour répertorier les comptes qu'elle engloble.
/// Elle ne peut pas contenir d'opérations bancaires, c'est-à-dire être impliquée dans des transactions.

class CompteVirtuel : public Compte_racine
{
    friend class CompteManager ;
protected:
    std::list<Compte_racine *> children_;
public:
    //constructeurs
    CompteVirtuel()=default;
    CompteVirtuel(const QString& n, type_compte t);


    void Add(Compte_racine * component) ;
    void RemoveChild(Compte_racine& component);
    /// \fn CompteVirtuel::IsVirtuel
    /// \brief permet de savoir si le compte en question est virtuel ou non, a été redéfinie par polymorphisme car cette méthode renvoie faux de base.
    bool IsVirtuel() const override { return true; }

    /// \fn CompteVirtuel::recup_children
    /// \brief renvoie la liste des "enfants" du compte virtuel en question, dans cette liste pourront être stockés à la fois des comptes classiques et virtuels.
    list<Compte_racine*>  recup_children()const{
        return children_;
    }

    CompteVirtuel* cloner() override;
};

///  \class CompteManager
/// \brief Classe compte manager qui gère les comptes
/// Elle a été implémentée avec le DP Singleton pour garantir son unicité dans le temps.
/// CompteManager contient une liste de compte classiques, une liste de comptes virtuels
/// L'unique instance de la classe gère ses propres objets
/// Instance unique récupérée via la méthode getManager()
/// Contient : opérateur d'égalité, get & set, méthodes de gestion, recherche & récupération des listes
/// En charge de la lecture du fichier XML compte

class CompteManager {
private :

    //structure handler pour le singleton
    struct handler {
        CompteManager *instance = nullptr ;

        ~handler() { delete instance; }
        handler() = default;
    };

    //attributs :
    list<Compte_racine*> all_comptes ;//comptes classiques
    list<Compte_racine*> comptes_virtuels; //les comptes virtuels
    double numeroCourant = 1; // le numéro de comptes
    static handler cm_handler ;

    CompteManager() = default ;
    ~CompteManager();

    //singleton = suppression de la duplication et de la recopie
    CompteManager(const CompteManager& tm) = delete;
    CompteManager& operator=(const CompteManager& tm) = delete;

public :
    //pour le trouver et le détruire
    static CompteManager& getManager();
    void libererManager();
    ///\fn CompteManager::getNumeroCourant
    /// \brief getter qui renvoie la valeur de numeroCourant, cet attribut sert à calculer automatiquement le numéro des comptes
    double getNumeroCourant(){return numeroCourant;}
    ///\fn CompteManager::incNumeroCourant
    /// \brief incrémente la valeur de numeroCourant, cet attribut sert à calculer automatiquement le numéro des comptes
    double incNumeroCourant(){return ++numeroCourant;}

    //méthodes get
    ///\fn CompteManager::get_liste_all_comptes
    /// \brief getter qui renvoie la liste de tous les Compte
    list<Compte_racine*>& get_liste_all_comptes(){return all_comptes ;}
    ///\fn CompteManager::get_liste_comptes_virtuels
    /// \brief getter qui renvoie la liste de tous les CompteVirtuel
    list<Compte_racine*>& get_liste_comptes_virtuels(){return comptes_virtuels ;}


    //méthodes de recherche
    Compte_racine * getCompteVirtuel(const QString & nom_recherche);//pour chercher un Virtuel
    Compte_racine * getCompte(const QString & nom_recherche);//pour chercher un classique
    Compte_racine * getAllCompte(const QString& nom_recherche); //chercher un classique ou un virtuel
    bool already_exists(const QString& nom_compte);

    //méthode de suppression des comptes dans les listes
    int Remove(Compte_racine* component);

    //méthode xml
    void load_comptes(const QString& f);

    ///\fn CompteManager::destroy_listes
    /// \brief permet d'effacer le contenu des deux listes all_comptes et comptes_virtuels.
    void destroy_listes(){
        all_comptes.clear();
        comptes_virtuels.clear();
    }
};


#endif // COMPTE_H_INCLUDED
