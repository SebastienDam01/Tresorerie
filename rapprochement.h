#ifndef RAPPROCHEMENT_H
#define RAPPROCHEMENT_H

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

#include "compte.h"
#include "transaction.h"
#include "state.h"
#include "mainwindow.h"

class transaction;
class MainWindow;


///\file rapprochement.h
/// \class Rapprochement
/// \brief Classe Rapprochement : Widget chargé de rapprocher les transactions des comptes classiques.

class Rapprochement : public QWidget {
    Q_OBJECT
public:
    explicit Rapprochement(QWidget *parent = nullptr, const QString& nom = "", MainWindow * mwindow = nullptr);
    ~Rapprochement();
    void setSolde(const double& solde) const;
    void setSolde_actuel(const double& solde) const;
    /// \fn Rapprochement::getDecision_rapproche
    /// \brief Permet de savoir si on a cliqué sur Oui ou Non dans rapprocher
    int getDecision_rapproche() const {return decision_rapproche;}
public slots:
    void setOui_rapproche();
    void setNon_rapproche();
    void setNon_rappV2();
    void valider_rap_triplet();
    void adding_triplet();
    void Fermer_Fenetre();
    void valider_non_rapproche();
    void demande_ajout_triplet();
    void closeEvent(QCloseEvent *event);
    void refresh();
private:
    MainWindow * mw;
    QString nom_compte;
    int decision_rapproche;
    transaction* transac_rapp;
    Compte* c;
    int ref;
    list<Compte*> deja_utilises;
    bool verification = false;
    bool premier_affichage;

    QLabel* affiche;
    QLabel* affiche_solde_actu;
    QLineEdit* solde_der_rapp;
    QLineEdit* solde_actuel;
    QLabel* question;
    QPushButton* oui;
    QPushButton* non;
    QPushButton *quitter;
    QHBoxLayout* affiche_layout;
    QHBoxLayout* solde_actuel_layout;
    QHBoxLayout* oui_non;
    QVBoxLayout* msgbox;
    QVBoxLayout* couche;

    QPushButton* valider_triplet;
    QPushButton* annuler;
    QHBoxLayout* annuler_valider_layout;
    QHBoxLayout* valider_triplet_layout;

    QHBoxLayout* question_triplet_layout;
    QLabel *question_triplet;
    QPushButton* oui_triplet;
    QPushButton* non_triplet;
    QHBoxLayout* triplet_oui_non_layout;
    QVBoxLayout* couche_triplet;

    QLabel* question2;
    QComboBox* ref_trans;
    QLabel* comptel;
    QComboBox* compte;
    QLabel* credl;
    QDoubleSpinBox* cred;
    QLabel* debl;
    QDoubleSpinBox* deb;
    QPushButton* valider;
    QHBoxLayout* quest_ref_layout;
    QHBoxLayout* compte_layout;
    QHBoxLayout* credit_layout;
    QHBoxLayout* debit_layout;
    QVBoxLayout* non_couche;

    // Affichage transactions non rapprochées
    QLabel* comptelabel;
    QComboBox* listeComptes;
    QPushButton* rapprocher_all;
    QLabel* soldel;
    QLineEdit* solde;
    QLabel* transfertsl;
    QTableWidget* vueTransactions;
    QVBoxLayout* liste_transac;
};

#endif // RAPPROCHEMENT_H
