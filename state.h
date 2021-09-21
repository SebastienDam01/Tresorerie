#ifndef STATE_H_INCLUDED
#define STATE_H_INCLUDED

#include "transaction.h"
#include <QDate>

///\file state.h


class transaction;

/// \class State
/// \brief Classe abstraite State, implentée suivant le DP State qui permet de gérer l'état rapproché ou non rapporché d'une transaction


class State{
public:
    virtual ~State(){}
    virtual void set_triplet(Compte & c, double cred, double deb, transaction *t)=0;
    virtual void set_date(const QDate &d, transaction *t)=0;
    virtual void set_reference(int ref, transaction *t)=0;
    virtual void set_titre(const QString & t, transaction *t_)=0;
    void setState(transaction &t, State *st);
};

/// \class Rapproche
/// \brief Classe Rapproche qui définit le premier état d'une transaction : rapprochée, donc non éditable

class Rapproche : public State {
public:
    void set_triplet(Compte & c, double cred, double deb, transaction *t);
    void set_date(const QDate &d, transaction *t);
    void set_reference(int ref, transaction *t);
    void set_titre(const QString & t, transaction *t_);
};

/// \class Non_Rapproche
/// \brief Classe Non_Rapproche qui définit le deuxième état d'une transaction : non rapprochée, donc éditable

class Non_Rapproche : public State {
public:
    void set_triplet(Compte & c, double cred, double deb, transaction *t);
    void set_date(const QDate &d, transaction *t);
    void set_reference(int ref, transaction *t);
    void set_titre(const QString & t, transaction *t_);
};

#endif // STATE_H_INCLUDED
