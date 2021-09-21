#ifndef ALL_TRANSACTIONS_H
#define ALL_TRANSACTIONS_H

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
#include "transaction.h"
#include "information.h"
#include "mainwindow.h"


/// \file all_transactions.h
/// \class all_transactions
/// \brief Widget qui permet d'afficher l'historique des transactions entrées par l'utilisateur. Ces transactions seront triées par date dans l'affichage.


class all_transactions : public QWidget {
Q_OBJECT

public:
explicit all_transactions(QWidget *parent = nullptr);
signals:
public slots:
private :
QLabel* transfertsl;
QTableWidget* historique_transac;
QVBoxLayout* couche;
};

bool cmp(transaction * t1,transaction * t2);

#endif // ALL_TRANSACTIONS_H
