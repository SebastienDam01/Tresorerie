#ifndef INFORMATION_H
#define INFORMATION_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLineEdit>
#include <QHeaderView>
#include <QDebug>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include "transaction.h"
#include "mainwindow.h"

class transaction;

/// \file information.h
/// \class Information
/// \brief Classe Information, c'est un Widget 'formulaire' qui permet de récolter les informations nécessaires pour ajouter ou modifier une transaction


class Information : public QWidget {
    Q_OBJECT
public:
    explicit Information(transaction * tr = nullptr ,QWidget *parent = nullptr);
    /// \fn Information::getCompte()
    /// \brief renvoie le nom du compte sur lequel l'utilisateur aura cliqué dans la QComboBox.
    const QString & getCompte() const{return compte->currentText();}
    /// \fn Information::getCredit()
    /// \brief renvoie le credit écrit par l'utilisateur dans la QDoubleSpinBox .
    /// \return double value
    double getCredit() const{return cred->value();}
    /// \fn Information::getDebit()
    /// \brief renvoie le debit écrit par l'utilisateur dans la QDoubleSpinBox .
    /// \return double value
    double getDebit() const{return deb->value();}
    /// \fn Information::~Information()
    /// \brief désalloue le pointeur sur la nouvelle transaction s'il a déjà été alloué (on stocke son clone).
    ~Information() { if(nouv != nullptr){delete nouv;}}
signals:
    void WidgetClosed();
public slots:
    void envoie_info();
    void suite_transaction();
private :
    int numero;
    transaction * old;
    transaction * nouv = nullptr;
    QLabel* datel;
    QDateEdit* date;
    QLabel* titrel;
    QLineEdit* titre;
    QLabel* comptel;
    QComboBox* compte;
    QLabel* credl;
    QDoubleSpinBox* cred;
    QLabel* debl;
    QDoubleSpinBox* deb;
    QPushButton* confirmer;
    QPushButton* suivant;
    QHBoxLayout* date_layout;
    QHBoxLayout* titre_layout;
    QHBoxLayout* compte_layout;
    QHBoxLayout* credit_layout;
    QHBoxLayout* debit_layout;
    QHBoxLayout* boutons;
    QVBoxLayout* couche;
};

#endif // INFORMATION_H
