#ifndef XmlRopidImportStream_H
#define XmlRopidImportStream_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtXml>
#include "sqlpraceropid.h"

class XmlRopidImportStream: public QObject
{
    Q_OBJECT
public:

    struct Navrat
    {
        QString nazevSloupce="";
        QString obsah="";
        QString typ="";
    };

    //konstruktor
    XmlRopidImportStream();


    //instance knihoven
    SqlPraceRopid ropidSQL;

    //promenne
    QString vstupniXmlSouborCesta;
    QDate platnostOd;
    QDate platnostDo;
    QVector<Navrat> polozkyKeVlozeni;

    //funkce
    int databazeStart();
    Navrat inicializujPolozku(QString nazevSloupce, QString obsah, QString typ);
    void natahniOld(QFile &file);
    void natahniNew(QFile &file);
    void otevriSoubor(QString cesta);

    QVector<int> seznamDlouhychSpoju(QDomNodeList &dlouheSpoje);
    int truncateTable(QString tabulka);
    int truncateAll();
    int vlozPlatnost(QDomElement koren, QDate &plOd, QDate &plDo);

signals:
    void odesliChybovouHlasku(QString chybovaHlaska);
private:

    QString overBoolean(QString vstup);
    QString overInteger(QString vstup);
    QString overString(QString vstup);
    QString slozInsert(QString nazevTabulky, QVector<Navrat>);
    QString vytvorCas(QString vstup);

    int vlozD(QDomElement koren);
    int vlozDd(QDomElement koren);
    int vlozCh(QDomElement koren);
    int vlozIds(QDomElement koren);
    int vlozK(QDomElement koren);
    int vlozL(QDomElement koren);
    int vlozM(QDomElement koren);
    int vlozO(QDomElement koren);
    int vlozP(QDomElement koren);
    int vlozPo(QDomElement koren);
    int vlozR(QDomElement koren);
    int vlozS(QDomElement koren);
    int vlozSpPo(QDomElement koren);
    int vlozT(QDomElement koren);
    int vlozTv(QDomElement koren);
    int vlozTy(QDomElement koren);
    int vlozX(QDomElement koren);
    int vlozZ(QDomElement koren);

    int vlozDNew(QXmlStreamAttributes atributy);
    int vlozDdNew(QXmlStreamAttributes atributy);
    int vlozDsNew(QXmlStreamAttributes atributyO,QXmlStreamAttributes atributyDs);
    int vlozChNew(QXmlStreamAttributes atributy);
    int vlozIdsNew(QXmlStreamAttributes atributy);
    int vlozLNew(QXmlStreamAttributes atributy);
    int vlozKNew(QXmlStreamAttributes atributy);
    int vlozMNew(QXmlStreamAttributes atributy);
    int vlozONew(QXmlStreamAttributes atributy);
    int vlozPNew(QXmlStreamAttributes atributy);
    int vlozPlatnostNew(QXmlStreamAttributes atributy, QDate &plOd, QDate &plDo);
    int vlozPoNew(QXmlStreamAttributes atributy);
    int vlozRNew(QXmlStreamAttributes atributy);
    QString vlozSNew(QXmlStreamAttributes atributy);
    int vlozSpPoNew(QXmlStreamAttributes atributy, QVector<int> navazneSpoje);
    int vlozTNew(QXmlStreamAttributes atributy);
    int vlozTvNew(QXmlStreamAttributes atributy);
    int vlozTyNew(QXmlStreamAttributes atributy);
    int vlozXNew(QXmlStreamAttributes atributy, int &counter, QString cisloSpoje);
    int vlozZNew(QXmlStreamAttributes atributy);

    QVector<int> seznamDlouhychSpojuNew(QXmlStreamAttributes atributy);

};

#endif // XmlRopidImportStream_H
