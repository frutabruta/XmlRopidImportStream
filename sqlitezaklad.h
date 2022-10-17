#ifndef SQLITEZAKLAD_H
#define SQLITEZAKLAD_H
#include <QObject>
#include <QtSql>


class SqLiteZaklad : public QObject
{
    Q_OBJECT
public:
    SqLiteZaklad();
    QSqlDatabase mojeDatabaze;

     QString nazevSouboru="";
     QString cesta="";
     QString cestaKomplet="";



    int pripoj();
    int otevriDB();
    int zavriDB();
    QString vytvorCasHodinyMinuty(QString vstup);
    QString vytvorCasHodinyMinutySekundy(QString vstup);
    QString doplnNulu(int cislo, int pocetMist);
    bool jeDatumVRozsahu(QDate datum, QDate zacatek, QDate konec);
    bool spustPrikaz(QString prikaz);
    bool existujeQueryChyba(QSqlQuery &dotaz);
    QString vektorStringuOddelovac(QVector<QString> vstup, QString oddelovac);
    bool zalozSqlTabulku(QString nazevTabulky, QVector<QString> sloupecky);
    bool vlozRadekDat(QString nazevTabulky, QVector<QString> hlavicka, QVector<QString> data);
    bool zrusSqlTabulku(QString nazevTabulky, QVector<QString> sloupecky);
    bool ukonciTransakci();
    bool zahajTransakci();
signals:
    void odesliChybovouHlasku(QString chybovaHlaska);

};

#endif // SQLITEZAKLAD_H
