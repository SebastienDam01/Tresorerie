#include "fichierxml.h"

/// \file fichierxml.cpp
/// \brief Fichiers de fonctions pour la classe fichierXML

FichierXML::Handler FichierXML::handler = FichierXML::Handler();


/// \fn FichierXML::getFichierXML
/// \brief Permet de récupérer l'instance unique de fichierXML
/// \return FichierXML& une référence sur la dite instance

FichierXML& FichierXML::getFichierXML()
{
    if(handler.instance == nullptr)
    {
        handler.instance = new FichierXML;
    }
    return *handler.instance;
}


/// \fn FichierXML::libererFichierXML
/// \brief Libération mémoire de l'instance de Fichier XML

void FichierXML::libererFichierXML()
{
    delete handler.instance;
    handler.instance = nullptr;
}


/// \fn recup_last_fichier
/// \brief Permet de récupérer les derniers fichiers ou de les créer si c'est la première fois que l'on démarre l'application

void recup_last_fichier(){
    QFile file("XMLfiles.txt");
    QString file_compte;
    QString file_transaction;
    if(file.open(QIODevice::ReadWrite | QIODevice::Text)){
        QTextStream flux(&file);

        if(file.size() > 0){
            file_compte = flux.readLine();
            file_transaction = flux.readLine();
        }
        else{ //premiere ouverture de l'app, le fichier vient d etre créé
            file_compte = ".export_comptes.xml";
            file_transaction = ".export_transactions.xml";
            flux << file_compte;
            flux << "\n";
            flux << file_transaction;

        }
        FichierXML & XML = FichierXML::getFichierXML();
        XML.setFichierCompte(file_compte);
        XML.setFichierTransaction(file_transaction);
    }
}

/// \fn change_fichiers_XML
/// \brief Permet de changer de fichiers en cours d'utilisation de l'application, la sauvegarde se fera sur ces fichiers


void change_fichiers_XML(){
    QString file_compte;
    QString file_transaction;
    file_compte = QFileDialog::getOpenFileName();
    file_transaction = QFileDialog::getOpenFileName();
    CompteManager & cm = CompteManager::getManager();
    cm.load_comptes(file_compte); //load des nouveaux fichiers
    transactionManager &tm=transactionManager::getManager();
    tm.load_transactions(file_transaction);

    QFile file("XMLfiles.txt"); //Puis sauvegarde pour prochaine ouverture
    if(file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate)){
        QTextStream flux(&file);
        flux << file_compte;
        flux << "\n";
        flux << file_transaction;
    }
}
