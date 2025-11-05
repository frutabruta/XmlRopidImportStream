#ifndef XMLIMPORTJR_H
#define XMLIMPORTJR_H


#include "xmlropidimportstream.h"

class XmlImportJr: public XmlRopidImportStream
{

    void run() override {
        QString result;
        /* ... here is the expensive or blocking operation ... */
        slotOtevriSoubor(vstupniXmlSouborCesta);
        emit resultReady(result);
    }
public:
    //konstruktor
    XmlImportJr();

    //promenne
    QString vstupniXmlSouborCesta;

    //funkce 
    //QString slozInsert(QString nazevTabulky, QVector<Navrat>); //public kvůli RopidUkolovnik
    int truncateAll();
    void otevriSoubor(QString cesta);
    int truncateApc();
    int truncateTimetables();
    int truncateTasks();

public slots:
    void slotOtevriSoubor(QString cesta);
signals:
         // void resultReady(const QString &s);
private:

    bool natahni(QFile &file);
    
    int vlozBodPol(QXmlStreamAttributes atributy, QXmlStreamAttributes atributyZast, QXmlStreamAttributes atributyPol, int &counter, QString nazevElementu);
    int vlozBodTraj(QXmlStreamAttributes atributy, QXmlStreamAttributes atributyTr, QXmlStreamAttributes atributyTraj, int &counter);
    int vlozD(QXmlStreamAttributes atributy);
    int vlozDd(QXmlStreamAttributes atributy);
    int vlozG(QXmlStreamAttributes atributy);
    int vlozCh(QXmlStreamAttributes atributy);
    int vlozIds(QXmlStreamAttributes atributy);
    int vlozK(QXmlStreamAttributes atributy);
    int vlozL(QXmlStreamAttributes atributy);
    int vlozM(QXmlStreamAttributes atributy);
    int vlozO(QXmlStreamAttributes atributy);
    int vlozObl(QXmlStreamAttributes atributy);
    int vlozP(QXmlStreamAttributes atributy);
    int vlozPlatnost(QXmlStreamAttributes atributy, QDate &plOd, QDate &plDo, QString fileName);
    int vlozPo(QXmlStreamAttributes atributy);
    int vlozR(QXmlStreamAttributes atributy);
    int vlozS(QXmlStreamAttributes atributy);
    int vlozSpPo(QXmlStreamAttributes atributy, QVector<int> navazneSpoje);
    int vlozT(QXmlStreamAttributes atributy);
    int vlozTv(QXmlStreamAttributes atributy);
    int vlozTy(QXmlStreamAttributes atributy);
    int vlozX(QXmlStreamAttributes atributy, int &counter, int cisloSpoje);
    int vlozWgs(QXmlStreamAttributes atributy, QXmlStreamAttributes atributyTr, QXmlStreamAttributes atributyTraj, int &counter);
    int vlozZ(QXmlStreamAttributes atributy);
    int seznamPoznamek(QXmlStreamAttributes atributy, int cisloSpoje, int poradi);

    QVector<int> seznamDlouhychSpoju(QXmlStreamAttributes atributy);
};

#endif // XMLIMPORTJR_H
