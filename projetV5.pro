QT += xml
QT += widgets
QT += core
CONFIG += c++11

SOURCES += \
    CapitauxPropres.cpp \
    all_rapprochements.cpp \
    all_transactions.cpp \
    bilan_viewer.cpp \
    choix_type.cpp \
    compte_resultat_viewer.cpp \
    fichierxml.cpp \
    formulaire_date.cpp \
    main.cpp \
    mainwindow.cpp \
    compte.cpp \
    rapprochement.cpp \
    transaction.cpp \
    state.cpp \
    transaction_viewer.cpp \
    information.cpp



HEADERS += \
    CapitauxPropres.h \
    all_rapprochements.h \
    all_transactions.h \
    bilan_viewer.h \
    choix_type.h \
    compte_resultat_viewer.h \
    fichierxml.h \
    formulaire_date.h \
    mainwindow.h \
    compte.h \
    rapprochement.h \
    transaction.h \
    state.h \
    transaction_viewer.h \
    information.h



FORMS += \
    mainwindow.ui
