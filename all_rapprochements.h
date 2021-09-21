#ifndef ALL_RAPPROCHEMENTS_H
#define ALL_RAPPROCHEMENTS_H

#include <QWidget>
#include <QLabel>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHeaderView>

#include "compte.h"

/// \file all_rapprochements.h
///\class all_rapprochements
///\brief Widget qui permet d'afficher l'historique des rapprochements avec la date de dernier rapprochement, le nom du compte et le solde de dernier rapprochement qui seront affichés.


class all_rapprochements : public QWidget {
    Q_OBJECT
public:
    explicit all_rapprochements(QWidget *parent = nullptr);
private :
    QLabel *rapprochement;
    QTableWidget *mes_rapprochements;
    QVBoxLayout* rapprochement_layout;
};




#endif // ALL_RAPPROCHEMENTS_H
