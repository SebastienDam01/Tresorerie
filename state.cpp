#include "state.h"

/// \file state.cpp
/// \brief Fichiers de fonctions pour la classe state


/// \fn State::setState
/// \brief Contructeur de la classe State, l'état est mis à non rapproché par dafaut
/// \param t la transaction à modifier
/// \param st le state associé à la transaction
/// \return Une instance de la classe State

void State::setState(transaction &t, State *st)
{
    State *stp = new Rapproche;
    if(t.currentState==nullptr){
        t.currentState= new Non_Rapproche;
    }
    else if(typeid(*(t.currentState)).name() == (typeid(*stp).name())){
        cout << "Transaction deja rapproche, on ne peut rechanger son etat" << endl;
        return;
    }
    else{
        State *aux=t.currentState;
        t.currentState=st;
        delete aux;
    }
}

/// \fn Non_Rapproche::set_triplet
/// \brief Modifier un triplet de transaction pour une transaction dont l'état n'est pas rapproché
/// \param c le compte concerné par la transaction
/// \param cred le montant du crédit
/// \param deb le montant du débit
/// \param t la transaction à modifier

void Non_Rapproche::set_triplet(Compte & c, double cred, double deb, transaction *t){
    for (auto it=t->get_tab_triplet().begin();it!=t->get_tab_triplet().end();it++){
        if(it->compte.getNom()==c.getNom()){
            it->compte=c;
            it->credit=cred;
            it->debit=deb;
        }
    }
}

/// \fn Non_Rapproche::set_date
/// \brief Modifier la date pour une transaction non rapprochée
/// \param d la date de la transaction à modifier
/// \param t la transaction à modifier


void Non_Rapproche::set_date(const QDate & d, transaction *t) {
    t->date=d;
}

/// \fn Non_Rapproche::set_reference
/// \brief Modifier la référence d'une transaction non rapprochée
/// \param ref la référence de la transaction à modifier
/// \param t la transaction à modifier

void Non_Rapproche::set_reference(int ref, transaction *t){
    t->reference=ref;
}

/// \fn Non_Rapproche::set_titre
/// \brief Modifier le titre d'une transaction non rapprochée
/// \param t le nom de la transaction à modifier
/// \param t_ la transaction à modifier

void Non_Rapproche::set_titre(const QString &t, transaction *t_){
    t_->titre=t;
}

/// \fn Rapproche::set_triplet
/// \brief Modifier un triplet de transaction pour une transaction dont l'état n'est pas rapproché
/// \param c le compte concerné par le triplet
/// \param cred le montant du crédit
/// \param deb le montant du débit
/// \param t la transaction à modifier

void Rapproche::set_triplet(Compte & c, double cred, double deb, transaction *t){
    cout << "Transaction rapproche, on ne modifie pas le triplet" << endl;
}

/// \fn Rapproche::set_date
/// \brief Ne pas modifier la date d'une transaction rapprochée
/// \param d la date
/// \param t la transaction à modifier

void Rapproche::set_date(const QDate &d, transaction *t){
    cout << "Transaction rapproche, on ne modifie pas la date" << endl;
}

/// \fn Rapproche::set_reference
/// \brief Ne pas modifier la référence d'une transaction rapprochée
/// \param ref la valeur de la référence
/// \param t la transaction à modifier

void Rapproche::set_reference(int ref, transaction *t){
    cout << "Transaction rapproche, on ne modifie pas la reference" << endl;
}

/// \fn Rapproche::set_titre
/// \brief Ne pas changer le titre d'une transaction rapprochée
/// \param t le nom à mettre
/// \param t_ la transaction à modifier

void Rapproche::set_titre(const QString & t, transaction *t_){
    cout << "Transaction rapproche, on ne modifie pas le titre" << endl;
}
