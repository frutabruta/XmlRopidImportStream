#ifndef XmlRopidImportStream_H
#define XmlRopidImportStream_H

#include <QtSql>
#include <QtXml>
#include <QDebug>
#include <QMainWindow>
#include <QCoreApplication>
#include <QApplication>
#include <QObject>
#include <QWidget>
#include <QtXml>
#include "sqlitezaklad.h"

class XmlRopidImportStream: public QThread
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
    SqLiteZaklad sqLiteZaklad;

    //promenne
    QString vstupniXmlSouborCesta;
    QStringList vstupniXmlSouborCesty;
    QDate platnostOd;
    QDate platnostDo;

    //funkce
    int databazeStart();
    Navrat inicializujPolozku(QString nazevSloupce, QString obsah, QString typ);

    void otevriSoubor(QString cesta);

    int truncateTable(QString tabulka);
    int truncateAll();

    int indexX=0;

    int spocitejRadkySouboru(QString fileName);
    void vlozPoleAtributu(QString nazevTabulky, QMap<QString, QString> atributy);
    void QMapAppend(QMap<QString, QString> &puvodni, QMap<QString, QString> pridany);
    int vacuum();
public slots:
 //   void slotOtevriSoubor(QString cesta);
signals:
    void odesliChybovouHlasku(QString chybovaHlaska);
    void signalNastavProgress(int vstup);
    void signalNastavProgressMax(int vstup);
    void resultReady(const QString &s);
protected:

//    void natahni(QFile &file);

    QString overBoolean(QString vstup);
    QString overBooleanInv(QString vstup);
    QString overInteger(QString vstup);
    QString overString(QString vstup);
    QString slozInsert(QString nazevTabulky, QVector<Navrat>);
    QString vytvorCas(QString vstup);



};

#endif // XmlRopidImportStream_H
