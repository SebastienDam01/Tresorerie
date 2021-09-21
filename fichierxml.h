#ifndef FICHIERXML_H
#define FICHIERXML_H
#include <QString>
#include <fstream>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include "compte.h"
#include "transaction.h"

//Singleton


/// \file fichierxml.h
/// \class FichierXML
/// \brief Classe FichierXML implémentée sous forme de Singleton, qui permet de créer ou d'ouvrir, puis de gérer les fichiers XML


class FichierXML
{
private:
    QString fichier_compte;
    QString fichier_transaction;
    FichierXML() = default;
    FichierXML(const FichierXML& f) = delete;
    struct Handler {
        FichierXML * instance = nullptr;
        ~Handler() { delete instance; instance = nullptr; }
        Handler() = default;
    };
    static Handler handler;
public:
    static FichierXML& getFichierXML();
    static void libererFichierXML();
    const QString& getFichierCompte() const {return fichier_compte;}
    const QString& getFichierTransaction() const {return fichier_transaction;}
    void setFichierCompte(const QString& f){fichier_compte = f;}
    void setFichierTransaction(const QString& f){fichier_transaction = f;}
};


void recup_last_fichier();
void change_fichiers_XML();

#endif // FICHIERXML_H
