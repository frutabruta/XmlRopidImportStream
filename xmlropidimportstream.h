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

    //funkce
    int databazeStart();
    Navrat inicializujPolozku(QString nazevSloupce, QString obsah, QString typ);

    void otevriSoubor(QString cesta);

    int truncateTable(QString tabulka);
    int truncateAll();

    int indexX=0;

public slots:
    void slotOtevriSoubor(QString cesta);
signals:
    void odesliChybovouHlasku(QString chybovaHlaska);
private:

    void natahniNew(QFile &file);

    QString overBoolean(QString vstup);
    QString overInteger(QString vstup);
    QString overString(QString vstup);
    QString slozInsert(QString nazevTabulky, QVector<Navrat>);
    QString vytvorCas(QString vstup);

    int vlozD(QXmlStreamAttributes atributy);
    int vlozDd(QXmlStreamAttributes atributy);
    int vlozCh(QXmlStreamAttributes atributy);
    int vlozIds(QXmlStreamAttributes atributy);
    int vlozK(QXmlStreamAttributes atributy);
    int vlozL(QXmlStreamAttributes atributy);
    int vlozM(QXmlStreamAttributes atributy);
    int vlozO(QXmlStreamAttributes atributy);
    int vlozP(QXmlStreamAttributes atributy);
    int vlozPlatnost(QXmlStreamAttributes atributy, QDate &plOd, QDate &plDo);
    int vlozPo(QXmlStreamAttributes atributy);
    int vlozR(QXmlStreamAttributes atributy);
    int vlozS(QXmlStreamAttributes atributy);
    int vlozSpPo(QXmlStreamAttributes atributy, QVector<int> navazneSpoje);
    int vlozT(QXmlStreamAttributes atributy);
    int vlozTv(QXmlStreamAttributes atributy);
    int vlozTy(QXmlStreamAttributes atributy);
    int vlozX(QXmlStreamAttributes atributy, int &counter, int cisloSpoje);

    int vlozZ(QXmlStreamAttributes atributy);
    int seznamPoznamek(QXmlStreamAttributes atributy, int cisloSpoje, int poradi);

    QVector<int> seznamDlouhychSpojuNew(QXmlStreamAttributes atributy);

};

#endif // XmlRopidImportStream_H
