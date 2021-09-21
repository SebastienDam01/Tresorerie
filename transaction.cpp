#include "transaction.h"

/// \file transaction.cpp
/// \brief Fichiers de fonctions pour la classe transaction

transactionManager::Handler transactionManager::handler = transactionManager::Handler();

/// \fn transactionManager::getManager
/// \brief Récupérer l'instance unique de TransactionManager avec cette méthode statique
/// \return transactionManager& l'instance du TransactionManager

transactionManager& transactionManager::getManager()
{
    if(handler.instance == nullptr)
    {
        handler.instance= new transactionManager;
    }
    return *handler.instance;
}


/// \fn transactionManager::libererManager
/// \brief Libérer la mémoire associée à l'instance unique

void transactionManager::libererManager()
{
    delete handler.instance;
    handler.instance = nullptr;
}

/// \fn transaction::transaction
/// \brief Constructeur avec paramètres de la classe transaction permettant de créer et d'initialiser une transaction
/// \param d date
/// \param ref référence
/// \param t titres

transaction::transaction(const QDate & d, const int ref,const QString & t): date(d),reference(ref),titre(t), currentState(new Non_Rapproche) {}

/// \fn transaction::transaction
/// \brief Constructeur par défaut de transaction

transaction::transaction():currentState(new Non_Rapproche){}

/// \fn transactionManager::~transactionManager
/// \brief destruction de transactionManager, permet de désallouer tout ce qui était présent dans la liste de transactions.


transactionManager::~transactionManager(){
    for(auto & it : transactions)
    {
        delete it;
    }
}

/// \fn transaction::set_triplet
/// \brief Pour set un triplet dans une transaction en fonction de son état
/// \param c le compte
/// \param cred le crédit
/// \param deb le débit
/// \param t la transaction

void transaction::set_triplet(Compte & c, double cred, double deb, transaction *t){
    currentState->set_triplet(c,cred,deb,this);
}

/// \fn transaction::set_date
/// \brief Pour set la date dans une transaction en fonction de son état
/// \param d la date
/// \param t la transaction concernée


void transaction::set_date(const QDate &d, transaction *t){
    currentState->set_date(d,this);
}


/// \fn transaction::set_reference
/// \brief Pour set une référence dans une transaction en fonction de son état
/// \param ref la référence
/// \param t la transaction

void transaction::set_reference(int ref, transaction *t){
    currentState->set_reference(ref,this);
}

/// \fn transaction::set_titre
/// \brief Pour set un titre dans une transaction en fonction de son état
/// \param t le titre
/// \param t_ la transaction

void transaction::set_titre(const QString & t, transaction *t_){
    currentState->set_titre(t,this);
}

/// \fn transaction::add_triplet
/// \brief Pour ajouter un triplet dans une transaction
/// \param c le compte du triplet
/// \param cred le crédit du triplet
/// \param deb le débit du triplet

void transaction::add_triplet(Compte & c, double cred,double deb)
{
    bool trouve = false;
    for(auto it=liste_triplet.begin(); it!=liste_triplet.end(); ++it)
    {
        if(it->get_compte().getNom()==c.getNom())
        {
            trouve = true;
        }
    }
    if(trouve == false)
    {
        triplet_transaction nouv(c,cred,deb);
        liste_triplet.push_front(nouv);
    }
}


/// \fn transactionManager::ajouterTransaction
/// \brief Ajouter une nouvelle transaction dans la liste de transactionManager
/// \param t la transaction
/// \return -1 si date inférieur à la date de dernier rapprochement d'un compte, -2 si pas de triplets dans la transaction, -3 si un unique triplet, -4 si débit et crédit nuls, -5 si debit = credit, -6 si transaction pas équilibrée, 0 si succès

int transactionManager::ajouterTransaction(transaction & t,bool load)
{
    State *pt1= new Rapproche;
    QDate date_dernier_rapprochement_compte_concerne(0,0,0);
    for(auto &it: t.get_tab_triplet()){
        if(it.get_compte().getDateDernierRapprochement() > date_dernier_rapprochement_compte_concerne)
        {
            date_dernier_rapprochement_compte_concerne = it.get_compte().getDateDernierRapprochement();
        }
    }
    if(t.get_date()<date_dernier_rapprochement_compte_concerne && load == false){
        return -1;
    }
    if(t.liste_triplet.size()==0)
    {
        cout << "Liste vide, impossible de créer une transaction" << endl;
        return -2;
    }
    if(t.liste_triplet.size()<2)
    {
        cout << "Impossible de creer une transaction avec un unique triplet, veuillez en créer au moins un autre" << endl;
        return -3;
    }
    else if(t.liste_triplet.size() >=2)
    {
        double somme=0;
        // verification debit credit, etc
        for(auto it = t.liste_triplet.begin(); it!=t.liste_triplet.end(); ++it)
        {
            // Règle 2
            if(it->get_credit() == 0 && it->get_debit() == 0)
            {
                cout<<"Credit et debit a 0"<<endl;
                return -4;
            }
            if(it->get_credit()<0)
            {
                it->set_debit(-(it->get_credit())); // Pour mettre la transaction au bon format
                it->set_credit(0);
                somme-=it->get_debit();
            }
            else if(it->get_debit()<0)
            {
                it->set_credit(-(it->get_debit())); // pour mettre la transaction au bon format
                it->set_debit(0);
                somme+=it->get_credit();

            }
            // Règle 3
            else if((it->get_credit()>0) && (it->get_debit()>0))
            {
                if(it->get_credit()>=it->get_debit())
                {
                    it->set_credit(it->get_credit()-it->get_debit()); // pour mettre la transacction au bon format
                    it->set_debit(0);
                    somme+=it->get_credit();
                }
                else if(it->get_debit()>it->get_credit())
                {
                    it->set_debit(it->get_debit()-it->get_credit()); // pour mettre la transaction au bon format
                    it->set_credit(0);
                    somme-=it->get_debit();
                }
            }
            else if(it->get_credit() > 0 && it->get_debit() == 0)
                somme += it->get_credit();
            else if(it->get_debit() > 0 && it->get_credit() == 0)
                somme -= it->get_debit();
            if(it->get_credit() == it->get_debit())
            {
                cout<<"Credit et debit egaux"<<endl;
                return -5;
            }
        }

        if(somme==0)
        {
            for(auto it = t.liste_triplet.begin(); it!=t.liste_triplet.end(); ++it)  // MAJ des comptes
            {
                if(it->get_credit()>0)
                {
                    it->get_compte().crediter(it->get_credit()); //On crédite le compte du bon montant
                    Compte_racine * aux = it->get_compte().getParent();
                }
                if(it->get_debit()>0)
                {
                    it->get_compte().debiter(it->get_debit());
                    Compte_racine * aux = it->get_compte().getParent();
                }
            }
            transactions.push_back(t.cloner());
            qDebug()<<"date "<<t.date<<" titre "<<t.titre<<" ref "<<t.reference;
            Ref++;
        }
        else
        {
            cout << "Desequilibre en debit et credit, veuillez revoir vos transactions" << endl;
            return -6;
        }
    }
    return 0;
}


/// \fn operator==
/// \brief Permet de vérifier l'égalité entre deux transactions seulement en fonction de l'attribut "référence"
/// \param a, une référence sur transaction
/// \param b, une référence sur transaction
/// \return boolean, true si les références sont égales, false sinon.

bool operator==(transaction& a,transaction& b){ //nécessaire pour appeler remove sur transactions.
    bool egal = true;
    if(a.get_reference() != b.get_reference()) egal = false;
    return egal;
    //attention ici je considere vraiment reference comme un clé primaire, il faut bien gérer derriere dans les autres fonctions pr pas que ca merde.
}

/// \fn operator==
/// \brief Permet de vérifier l'égalité entre deux transactions const seulement en fonction de l'attribut "référence"
/// \param a, une référence const sur transaction
/// \param b, une référence const sur transaction
/// \return boolean, true si les références sont égales, false sinon.

bool operator==(const transaction& a,const transaction& b){ //nécessaire pour appeler remove sur transactions.
    bool egal = true;
    if(a.get_reference() != b.get_reference()) egal = false;
    return egal;
    //attention ici je considere vraiment reference comme un clé primaire, il faut bien gérer derriere dans les autres fonctions pr pas que ca merde.
}

/// \fn transactionManager::annuler_transaction
/// \brief Annuler une transaction contenue dans la liste de transactionManager
/// \param old_t la transaction à annuler si elle existe

void transactionManager::annuler_transaction(transaction& old_t){
    for(auto it = old_t.liste_triplet.begin(); it!=old_t.liste_triplet.end(); ++it){
        Compte& temp_compte = it->get_compte();
        temp_compte.crediter(it->get_debit()); //on fait l'inverse des opérations faite dans la transation qu'on veut annuler
        temp_compte.debiter(it->get_credit());

    }
    transactions.remove(&old_t);
}


/// \fn cloture
/// \brief Cloture du livre
/// Sommer les dépenses, les recettes, transférer vers un Compte Résultat puis un Compte Excédent ou Déficit
/// \param w parent

void cloture(QWidget * w)
{
    CompteManager& cm = CompteManager::getManager();
    transactionManager & tm = transactionManager::getManager();
    list<Compte_racine*> l = cm.get_liste_all_comptes();
    //bool format_mauvais=true;
    QDate date = QDate::currentDate();

    double solde_depenses = 0;
    double solde_recettes = 0;
    QString titre_transaction;
    //A quelle transaction l'utilisateur peut-il donner un titre ?


    int oui_ou_non = QMessageBox::question(w,"Demande","Voulez vous entrer un titre pour la transaction de cloture ?",QMessageBox ::Yes | QMessageBox::No |QMessageBox::Cancel);
    if(oui_ou_non == QMessageBox::Yes)
    {
        bool cancel;
        titre_transaction = QInputDialog::getText(w, "titre", "Entrez le titre désiré :", QLineEdit::Normal, "", &cancel);
        if(!cancel){
            return;
        }
    }
    else if(oui_ou_non == QMessageBox::No)
    {
        titre_transaction = "Cloture du livre" ;
    }
    else{
        return;
    }

    Compte* resultat;
    Compte_racine *c = cm.getCompte("Resultat");
    if(c == nullptr)
    {
        Compte res("Resultat",Passifs);
        resultat = dynamic_cast<Compte*>(cm.getCompte("Resultat"));
    }
    else{
        if(c->getType() == Passifs && c->getParent() == nullptr)
            resultat= dynamic_cast<Compte*>(c);
        else{
            Compte res("Resultat",Passifs);
            resultat = dynamic_cast<Compte*>(cm.getCompte("Resultat"));
        }
    }
    Compte * excedent;
    c = cm.getCompte("Excedent");
    if(c == nullptr){
        Compte exc("Excedent",Passifs);
        excedent= dynamic_cast<Compte*>(cm.getCompte("Excedent"));
    }
    else{
        if(c->getType() == Passifs && c->getParent() == nullptr)
            excedent = dynamic_cast<Compte*>(c);
        else{
            Compte exc("Excedent",Passifs);
            excedent= dynamic_cast<Compte*>(cm.getCompte("Excedent"));
        }
    }

    Compte * deficit;
    c = cm.getCompte("Deficit");
    if(c == nullptr){
        Compte def("Deficit",Passifs);
        deficit = dynamic_cast<Compte*>(cm.getCompte("Deficit"));
    }
    else{
        if(c->getType() == Passifs && c->getParent() == nullptr)
            deficit = dynamic_cast<Compte*>(c);
        else{
            Compte def("Deficit",Passifs);
            deficit = dynamic_cast<Compte*>(cm.getCompte("Deficit"));
        }
    }

    transaction t_depenses(date, tm.recupRef(),titre_transaction);
    transaction t_recettes(date, tm.recupRef()+1,titre_transaction);
    transaction t_EouD(date, tm.recupRef()+2,titre_transaction);

    for(auto & it : l)
    {
        if (it->getType() == Depenses)
        {
            qDebug()<<"--------- compte :"<<it->getNom()<<"solde : "<<it->getSolde();
            if(it->getSolde() != 0){
                solde_depenses += it->getSolde();
                Compte * c = dynamic_cast<Compte*>(it);
                t_depenses.add_triplet(*c,it->getSolde(),0);
            }
        }
        else if (it->getType() == Recettes)
        {
            if(it->getSolde() != 0){
                solde_recettes += it->getSolde();
                Compte * c = dynamic_cast<Compte*>(it);
                t_recettes.add_triplet(*c,0,it->getSolde());
            }

        }
    }
    t_recettes.add_triplet(*resultat,solde_recettes,0 );
    tm.ajouterTransaction(t_recettes);

    t_depenses.add_triplet(*resultat,0, solde_depenses);
    tm.ajouterTransaction(t_depenses);

    //Si le solde de resultat est positif, on le met dans excedent, sinon dans deficit
    if(resultat->getSolde() >= 0)
    {
        t_EouD.add_triplet(*excedent, resultat->getSolde(), 0);
        t_EouD.add_triplet(*resultat,0,resultat->getSolde());
    }
    else
    {
        t_EouD.add_triplet(*deficit,resultat->getSolde(), 0);
        t_EouD.add_triplet(*resultat, 0, resultat->getSolde());
    }
    tm.ajouterTransaction(t_EouD);
}


/// \fn transactionManager::rapprocher
/// \brief Rapprocher un compte, cette fonction permet notamment de calculer le solde de dernier rapprochement en fonction de si le compte avait déjà été rapproché ou non.
/// S'il n'a jamais été rapproché, on prendra le solde initial(s'il existe), autrement on calculera en fonction de toutes les transactions qui ont déjà été rapprochées.
/// Elle calcule aussi le solde actuel du compte, c'est-à-dire le solde généré par les nouvelles transactions depuis le dernier rapprochement s'il y en a.
/// Cette fonction lancera également le widget Rapprochement
/// \param nom_compte
/// \param mwindow
void transactionManager::rapprocher(const QString & nom_compte, MainWindow *mwindow)
{
    //Si compte virtuel sélectionné
    string virtuel = nom_compte.toStdString();
    if(virtuel.find("[Virtuel]")!=string::npos){
        qDebug() << "Compte virtuel" << endl;
        QMessageBox msgBox;
        msgBox.setText("Compte virtuel, veuillez sélectionner un compte non virtuel.");
        msgBox.exec();
    }
    else {
        // Trouvons le solde du dernier rapprochement
        double solde_der_rapp=0;
        State *pt1= new Rapproche;
        State *pt2= new Non_Rapproche;
        bool trouve_rap=false;
        bool trouve_transac=false;
        QDate date_temoin(0,0,0);
        CompteManager& cm = CompteManager::getManager();
        list<Compte_racine*> liste_compte_classique = cm.get_liste_all_comptes();
        if(cm.getCompte(nom_compte)->getDateDernierRapprochement() == QDate(0,0,0)){
            for(auto it=transactions.begin(); it!=transactions.end(); ++it)
            {
                for(auto it_=(*it)->liste_triplet.begin(); it_!=(*it)->liste_triplet.end(); ++it_){
                    if(it_->get_compte().getNom()==nom_compte){
                        if((*it)->get_titre() == "Solde initial"){
                            if(it_->get_credit() > 0)
                            {
                                if(it_->get_compte().getType() == Passifs || it_->get_compte().getType() == Recettes){
                                    solde_der_rapp += it_->get_credit();
                                }
                                else{
                                    solde_der_rapp -= it_->get_credit();
                                }
                            }
                            if(it_->get_debit() > 0)
                            {
                                if(it_->get_compte().getType() == Actifs ||it_->get_compte().getType() == Depenses){
                                    solde_der_rapp += it_->get_debit();
                                }
                                else{
                                    solde_der_rapp -= it_->get_debit();
                                }
                            }
                        }

                    }
                }
            }
        }
        else{
            for(auto it=transactions.begin(); it!=transactions.end(); ++it)
            {
                if (typeid(*((*it)->currentState)).name() == typeid(*pt1).name())
                {
                    for(auto it_=(*it)->liste_triplet.begin(); it_!=(*it)->liste_triplet.end(); ++it_)
                    {
                        // si on trouve une liste de triplet correspondante, on la montre à l'utilisateur et on lui demande si il veut rapprocher ou pas
                        if(it_->get_compte().getNom()==nom_compte)
                        {
                            if(it_->get_credit() > 0)
                            {
                                if(it_->get_compte().getType() == Passifs || it_->get_compte().getType() == Recettes){
                                    solde_der_rapp += it_->get_credit();
                                }
                                else{
                                    solde_der_rapp -= it_->get_credit();
                                }
                            }
                            if(it_->get_debit() > 0)
                            {
                                if(it_->get_compte().getType() == Actifs ||it_->get_compte().getType() == Depenses){
                                    solde_der_rapp += it_->get_debit();
                                }
                                else{
                                    solde_der_rapp -= it_->get_debit();
                                }
                            }
                            trouve_rap=true;
                        }
                    }
                }
            }
        }
        Compte * c = dynamic_cast<Compte*>(cm.getCompte(nom_compte));
        for(auto & it : transactions)
        {
            if(c->getDateDernierRapprochement() < it->get_date()){
                for(auto & it_ : it->liste_triplet)
                {
                    if(it_.get_compte().getNom()==nom_compte)
                    {
                        {
                            trouve_transac = true;
                            break;
                        }
                    }
                }
            }
        }
        for(auto it=transactions.begin(); it!=transactions.end(); ++it)
        {
            for(auto it_=(*it)->liste_triplet.begin(); it_!=(*it)->liste_triplet.end(); ++it_)
            {
                if(it_->get_compte().getNom()==nom_compte)
                {
                    if (typeid(*((*it)->currentState)).name() == typeid(*pt2).name())
                    {
                        trouve_rap = true;
                        break;
                    }
                }
            }
        }
        if(trouve_transac==false)
        {
            qDebug() << "Compte introuvable" << endl;
            QMessageBox msgBox;
            msgBox.setText("Veuillez sélectionner un autre compte. Celui-ci est déjà rapproché ou ne contient pas de transactions.");
            msgBox.exec();
        }
        else{
            Rapprochement* rap = new Rapprochement(nullptr, nom_compte,mwindow);
            rap->setAttribute(Qt::WA_DeleteOnClose);
            rap->setSolde(solde_der_rapp);

            double solde_actuel=0;
            for (auto it=liste_compte_classique.begin(); it!=liste_compte_classique.end(); ++it)
            {
                if((*it)->getNom()==nom_compte) // La première étoile me permet d'accéder au pointeur
                {
                    solde_actuel=(*it)->getSolde();
                    break;
                }
            }
            rap->setSolde_actuel(solde_actuel);
            // decision rapprochement
            rap->show();
            //return solde_der_rapp;
        }
    }
}

/// \fn transactionManager::load_transactions
/// \brief Charger le transactions contenues dans les fichiers xml
/// \param f le nom du fichier

void transactionManager::load_transactions(const QString& f){
    State *pt1= new Rapproche;
    State *pt2= new Non_Rapproche;
    int max_ref = 0;
    //Création du fichier à partir du nom fourni
    QFile file(f);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug()<<"erreur ouverture lecture fichier compte";
    }
    // création du document et parsing du fichier xml
    QDomDocument xmlDoc;
    xmlDoc.setContent(&file); // on met le contenu du fichier dans xmlDoc

    transactionManager &tm=transactionManager::getManager();

    QDomNodeList transaction_d = xmlDoc.elementsByTagName("Transaction_");
    // identifie toutes les balises où il y a Transaction_ au début
    transactions.clear();
    for (int i = 0; i < transaction_d.size(); i++)
    {
        QDomElement transac_node = transaction_d.item(i).toElement();

        QString titre = transac_node.attribute("titre", "");
        QString reference = transac_node.attribute("reference", "");
        int ref=reference.toInt();
        QString date = transac_node.attribute("date", ""); // on recupere le nom du parent
        qDebug() << "Date lue" << date;
        QDate date_= QDate::fromString(date, "dd/MM/yyyy");
        qDebug() << "Date convertie en QDate à partir de la lecture" <<date_;
        transaction t(date_,ref,titre); // le contenu de la liste ne sera pas désalloué à la sortie de la fonction grâce au pointeur
        QString state = transac_node.attribute("state", "");
        if (state == typeid(*pt1).name()){
            t.get_current_state()->setState(t, new Rapproche);
        } else if(state == typeid(*pt2).name()){
            t.get_current_state()->setState(t, new Non_Rapproche);
        }
        QDomElement triplet_fils=transac_node.firstChild().toElement();
        int j=0;
        while(!triplet_fils.isNull()){
            j++;
            QString nom_compte=triplet_fils.attribute("Nom_Compte", "");
            QString credit=triplet_fils.attribute("Credit","");
            QString debit=triplet_fils.attribute("Debit","");
            double credit_d=credit.toDouble();
            double debit_d=debit.toDouble();

            CompteManager& cm = CompteManager::getManager();
            Compte_racine *c = cm.getCompte(nom_compte);
            Compte *compte_classique=dynamic_cast<Compte *>(c);
            t.add_triplet(*compte_classique,credit_d,debit_d);
            qDebug()<<"triplet : "<<compte_classique->getNom()<<" cred : "<<credit_d<<" deb "<<debit_d;
            triplet_fils=triplet_fils.nextSibling().toElement();
        }
        if(max_ref < ref)
            max_ref = ref;
        tm.ajouterTransaction(t,true);
    }
    tm.setRef(max_ref+1);
    file.close();
}

/// \fn transaction::~transaction
/// \brief Destructeur de notre class transaction.
/// Permet de vider la liste des triplets.

transaction::~transaction(){
    liste_triplet.clear();
    list<triplet_transaction>().swap(liste_triplet); //swap with an empty list
}
