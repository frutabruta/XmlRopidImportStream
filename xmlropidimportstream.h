#ifndef XmlRopidImportStream_H
#define XmlRopidImportStream_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QtXml>
#include "sqlitezaklad.h"

class XmlRopidImportStream: public QThread
{
    Q_OBJECT

    void run() override {
        QString result;
        /* ... here is the expensive or blocking operation ... */
        slotOtevriSoubor(vstupniXmlSouborCesta);
        emit resultReady(result);
    }
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
    SqLiteZaklad ropidSQL;

    //promenne
    QString vstupniXmlSouborCesta;
    QDate platnostOd;
    QDate platnostDo;

    //funkce
    int databazeStart();
    Navrat inicializujPolozku(QString nazevSloupce, QString obsah, QString typ);

    void otevriSoubor(QString cesta);

    QString slozInsert(QString nazevTabulky, QVector<Navrat>); //public kvůli RopidUkolovnik

    int truncateTable(QString tabulka);
    int truncateAll();

    int indexX=0;

    int spocitejRadkySouboru(QString fileName);




public slots:
    void slotOtevriSoubor(QString cesta);
signals:
    void odesliChybovouHlasku(QString chybovaHlaska);
    void signalNastavProgress(int vstup);
    void signalNastavProgressMax(int vstup);
    void resultReady(const QString &s);
private:

    bool natahni(QFile &file);

    QString overBoolean(QString vstup);
      QString overBooleanInv(QString vstup);
    QString overInteger(QString vstup);
    QString overString(QString vstup);

    QString vytvorCas(QString vstup);

    int vlozBod(QXmlStreamAttributes atributy, QXmlStreamAttributes atributyTr, QXmlStreamAttributes atributyTraj, int &counter);
    int vlozD(QXmlStreamAttributes atributy);
    int vlozDd(QXmlStreamAttributes atributy);
    int vlozG(QXmlStreamAttributes atributy);
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

    QVector<int> seznamDlouhychSpoju(QXmlStreamAttributes atributy);


};

#endif // XmlRopidImportStream_H
