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

    int pripoj();
    int otevriDB();
    int zavriDB();
    QString vytvorCasHodinyMinuty(QString vstup);
    QString vytvorCasHodinyMinutySekundy(QString vstup);
    QString doplnNulu(int cislo, int pocetMist);
    bool jeDatumVRozsahu(QDate datum, QDate zacatek, QDate konec);
signals:
    void odesliChybovouHlasku(QString chybovaHlaska);

};

#endif // SQLITEZAKLAD_H
