#ifndef TRANSACTION_H_INCLUDED
#define TRANSACTION_H_INCLUDED

#include "compte.h"
#include <typeinfo>

#include "state.h"
#include "rapprochement.h"
#include <QDate>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QErrorMessage>

class MainWindow;


/// \file transaction.h
/// \brief Fichier qui définit la classe transaction et son manager



/// \class triplet_transaction
/// \brief Classe triplet_transaction qui contient un compte, un crédit et un débit, et sera contenue par un objet transaction


class triplet_transaction{
    friend class Non_Rapproche;
private:
    Compte & compte;
    double credit;
    double debit;
public:
    /// \fn triplet_transaction::triplet_transaction
    /// \brief Constructeur qui prend en paramètre un Compte, et deux double
    /// Normalment le crédit ou le débit sera nul si l'autre ne l'est pas. Sinon ceux-ci seront recalculés conformément aux règles énoncées dans le sujet.
    /// \param Compte & c, le Compte représentant le compte associé
    /// \param const double deb, le Débit représentant le montant porté au débit de ce compte dans la transaction
    /// \param const double cred, le Crédit représentant le montant porté au crédit de ce compte dans la transaction

    triplet_transaction(Compte & c, const double cred,const double deb) : compte(c),credit(cred),debit(deb){}
     /// \fn triplet_transaction::get_compte
     /// \brief getter qui renvoie le compte
     /// \return Compte &

    Compte & get_compte() {return compte;}

    /// \fn triplet_transaction::get_credit
    /// \brief getter qui renvoie le crédit associé au triplet
    /// \return double

    double get_credit()const{return credit;}

    /// \fn triplet_transaction::get_debit
    /// \brief getter qui renvoie le débit associé au triplet
    /// \return double

    double get_debit()const{return debit;}

    /// \fn triplet_transaction::set_credit
    /// \brief setter qui modifie la valeur du crédit associé au triplet

    void set_credit(double m ) { credit=m;}

    /// \fn triplet_transaction::set_debit
    /// \brief setter qui modifie la valeur du débit associé au triplet

    void set_debit (double m) { debit=m;}

};


class State;


/// \class transaction
/// \brief Classe transaction qui utilisé pour modéliser une opération bancaire
/// Implémentée selon le DP Factory method et le DP State
/// Objets gérés par TransactionManager
/// Utilise une liste d'objets triplet_transaction


class transaction{
    friend class transactionManager;
    friend class State;
    friend class Non_Rapproche;
    QDate date;
    int reference;
    QString titre;
    list<triplet_transaction> liste_triplet; // Liste chaînée liste_triplet de type triplet_transaction
    State * currentState;
public:
    transaction(const QDate & d, const int ref,const QString & t);
    transaction();
    ~transaction();
    void add_triplet(Compte & c, double cred,double deb);

    ///\fn transaction::get_date
    ///\brief getter qui renvoie la date de la transaction
    /// \return const QDate &
    const QDate & get_date() const{return date;}
    ///\fn transaction::get_reference
    ///\brief getter qui renvoie la référence de la transaction
    /// \return int
    int get_reference() const{return reference;}
    ///\fn transaction::get_titre
    ///\brief getter qui renvoie le titre de la transaction
    /// \return const QString &
    const QString & get_titre() const{return titre;}
    ///\fn transaction::get_tab_triplet
    ///\brief getter qui renvoie la liste de triplet de la transaction
    /// \return list<triplet_transaction>
    list<triplet_transaction> get_tab_triplet() const {return liste_triplet;}
    ///\fn transaction::get_current_state
    ///\brief getter qui renvoie l'état de la transaction (Rapproche ou Non_Rapproche)
    ///\return State*
    State* get_current_state()const{return currentState;}

    void set_triplet(Compte & c, double cred, double deb, transaction *t);
    void set_date(const QDate &d, transaction *t); // ok
    void set_reference(int ref, transaction *t);// ok
    void set_titre(const QString & t, transaction *t_);// ok

    ///\fn transaction::cloner
    ///\brief Fonction qui permet de cloner une transaction selon le DP Factory Method
    ///\return transaction*
    transaction* cloner(){
        transaction* nouveau = new transaction(*this);
        return nouveau ;
    }
    ///\fn transaction::operator<
    ///\param const transaction & t
    ///\brief redéfinition de l'opérateur < en comparant la date des transactions
    ///\return transaction*
    bool operator<(const transaction & t) const{
        return date<t.get_date();
    }

};


/// \fn bool operator==
/// \brief Surcharge l'operateur == pour les transactions
/// \param a et b, deux références sur des transactions.
/// \return bool


bool operator==(transaction& a, transaction& b);

/// \class TransactionManager
/// \brief Classe TransactionManager qui gère les objets de la classe transaction
/// Implémentée sous forme de Singleton, exploite le DP Factory Method de transaction
/// Contient une liste chainée de transactions et les gère (ajout, modification, suppression, recherche...)

class transactionManager {

private:
    list<transaction*> transactions;
    struct Handler {
        transactionManager* instance = nullptr;
        ~Handler() { delete instance; instance = nullptr; }
        Handler() = default;
    };
    static Handler handler;
    transactionManager() = default;
    ~transactionManager();
    transactionManager(const transactionManager& tm) = delete;
    transactionManager& operator=(const transactionManager& tm) = delete;
    int Ref = 0;
public:
    static transactionManager& getManager();
    static void libererManager();
    ///\fn transactionManager::setRef
    ///\brief setter pour modifier la valeur de l'attribut Ref qui se charge de calculer automatiquement la référence d'une transaction
    void setRef(int r){Ref = r;}
    int ajouterTransaction(transaction & t, bool load = false);
    ///\fn transactionManager::recupRed
    ///\brief getter qui renvoie la valeur de l'attribut Ref qui se charge de calculer automatiquement la référence d'une transaction
    int recupRef()const {return Ref;}
    void annuler_transaction(transaction& old_t);
    void rapprocher(const QString & nom_compte, MainWindow * mwindow);
    ///\fn transactionManager::get_tab_transactions
    ///\brief getter qui renvoie la liste de transactions stockée par notre logiciel
    list<transaction *> get_tab_transactions() const {return transactions;}
    void load_transactions(const QString& f);
    ///\fn transactionManager::detroy_transac
    ///\brief Permet de vider la liste de transactions
    void detroy_transac(){transactions.clear();}
};

///\fn void cloture
/// \brief Permet la cloture du livre
/// \param w le widget parent
/// \return Void


void cloture(QWidget * w);


#endif //TRANSACTION_H_INCLUDED
