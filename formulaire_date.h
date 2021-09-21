#ifndef FORMULAIRE_DATE_H
#define FORMULAIRE_DATE_H

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
#include <QDateEdit>
#include "compte_resultat_viewer.h"


/// \file formulaire_date.h
/// \class formulaire_date
/// \brief Classe formulaire_date, Widget qui permet de saisir une plage de dates pour générer le compte de résultat

class formulaire_date : public QWidget {
    Q_OBJECT
public slots:
    void generate();
public:
    explicit formulaire_date(QWidget *parent = nullptr);
    /// \fn formulaire_date::get_date_debut
    /// \brief Permet de renvoyer l'attribut date d1, la date de début
    QDate get_date_debut(){
        return d1;
    }
    /// \fn formulaire_date::get_date_debut
    /// \brief Permet de renvoyer l'attribut date d2, la date de fin
    QDate get_date_fin(){
        return d2;
    }
private :
    QDate d1;
    QDate d2;

    QLabel *date_debut;
    QLabel *date_fin;
    QDateEdit* date_ecrite1;
    QDateEdit* date_ecrite2;

    QHBoxLayout* date_layout1;
    QHBoxLayout* date_layout2;
    QVBoxLayout* layout_glob;

    QPushButton *generateur_resultat_viewer;
};

#endif // FORMULAIRE_DATE_H
