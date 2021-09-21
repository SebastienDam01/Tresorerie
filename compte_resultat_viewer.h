#ifndef COMPTE_RESULTAT_VIEWER_H
#define COMPTE_RESULTAT_VIEWER_H

#include "transaction_viewer.h"


/// \file compte_resultat_viewer.h
/// \class compte_resultat_viewer
/// \brief Classe compte_resultat_viewer qui crée le relevé des recettes et des dépenses (compte de résultat) pour la periode courante
/// Dans la partie supérieure supérieure du widget, les résultats sur les recettes et dans la partie inférieure les résultats sur les dépenses.
/// Notons que le solde des comptes virtuels ne sera pas affiché, nous avons fait ce choix pour plus de visibilité.
/// Le constructeur gère tout


class compte_resultat_viewer : public QWidget {
    Q_OBJECT
public:
    explicit compte_resultat_viewer(QDate d1, QDate d2, QWidget *parent = nullptr);

private :
    QLabel *total;
    QLabel *total_recettes;
    QLabel *total_depenses;


    QLineEdit *solde;
    QLineEdit *solde_recette;
    QLineEdit *solde_depense;


    QTableWidget *bilan_recettes;
    QTableWidget *bilan_depenses;

    QVBoxLayout* bilan_layout;

    QHBoxLayout* solde_total_layout;
    QHBoxLayout* solde_t_r_layout;
    QHBoxLayout* solde_t_d_layout;
};

#endif // COMPTE_RESULTAT_VIEWER_H
