#ifndef BILAN_VIEWER_H
#define BILAN_VIEWER_H

#include "transaction_viewer.h"

/// \file bilan_viewer.h
///\class bilan_viewer
/// \brief Widget qui permet d'afficher le bilan comptable
/// Dans la partie supérieure supérieure du widget, nous retrouvons les comptes d'actifs avec leur solde et dans la partie inférieure les comptes de passifs avec leur solde.
/// Notons que le solde des comptes virtuels ne sera pas affiché, nous avons fait ce choix pour plus de visibilité.


class bilan_viewer : public QWidget {
    Q_OBJECT
public:
    explicit bilan_viewer(QWidget *parent = nullptr);
private :
    QLabel *total;
    QLabel *total_actif;
    QLabel *total_passif;

    QLineEdit *solde;
    QLineEdit *solde_actif;
    QLineEdit *solde_passif;

    QTableWidget *bilan_actif;
    QTableWidget *bilan_passif;

    QVBoxLayout* bilan_layout;
    QHBoxLayout* solde_total_layout;
    QHBoxLayout* solde_t_p_layout;
    QHBoxLayout* solde_t_a_layout;
};


#endif // BILAN_VIEWER_H
