#include "sqlitezaklad.h"

SqLiteZaklad::SqLiteZaklad()
{

}


int SqLiteZaklad::pripoj()

{
    qDebug()<< Q_FUNC_INFO;
    this->mojeDatabaze = QSqlDatabase::addDatabase("QSQLITE");
    //this->mojeDatabaze.setHostName(adresa);
    //this->mojeDatabaze.setPort(3306);
    //this->mojeDatabaze.setHostName("127.0.0.1");
    QString nazevSouboru="data.sqlite";
    QString cesta=QCoreApplication::applicationDirPath()+"/"+nazevSouboru;
    qDebug()<<"cesta:"<<cesta;
    this->mojeDatabaze.setDatabaseName(cesta);
    bool ok = otevriDB();
    if (ok==true)
    {
        qDebug()<<"připojení se povedlo";
        //emit odesliChybovouHlasku("připojení se povedlo");
        return 1;
    }
    else
    {
        emit odesliChybovouHlasku("připojení se nezdařilo");
        qDebug()<<"nepovedlo se";
        return 0;
    }

}

/*!

*/
int SqLiteZaklad::otevriDB()
{
    qDebug()<< Q_FUNC_INFO;
    if(this->mojeDatabaze.open())
    {
        qDebug()<<"podarilo se pripojit k databazi ROPID";
        qDebug()<<"is driver available "<<QString::number(mojeDatabaze.isDriverAvailable("QSQLITE"));
        qDebug()<<"je databaze otevrena "<<QString::number(mojeDatabaze.isOpen());
        //emit odesliChybovouHlasku("je databaze otevrena");
        qDebug()<<"je databaze validni "<<QString::number(mojeDatabaze.isValid());
        return 1;
    }
    else
    {
        emit odesliChybovouHlasku("DB se nepovedlo otevrit "+mojeDatabaze.lastError().text());
        qDebug()<<"DB se nepovedlo otevrit "<<mojeDatabaze.lastError();

    }
    return 0;
}



/*!

*/
int SqLiteZaklad::zavriDB()
{
    qDebug()<< Q_FUNC_INFO;
    this->mojeDatabaze.close();
    if(1)
    {
        QString chybaZavriDb="DB se povedlo zavřít";
        emit odesliChybovouHlasku(chybaZavriDb);
        qDebug()<<chybaZavriDb;

        return 1;
    }
    else
    {
        QString chybaZavriDb="DB se nepovedlo zavřít";
        emit odesliChybovouHlasku(chybaZavriDb);
        qDebug()<<chybaZavriDb;

    }
    return 0;
}

/*!

*/
QString SqLiteZaklad::vytvorCasHodinyMinuty(QString vstup)
{
    // qDebug()<<"SqlPraceRopid::vytvorCas";
    int cislo=vstup.toInt();
    cislo=cislo%86400; //oprava pro spoje jedoucí přes půlnoc
    int hodiny=cislo/3600;
    int minuty=(cislo%3600)/60;
    vstup="";
    vstup=QString::number(hodiny)+":"+doplnNulu( minuty,2);
    return vstup;
}


/*!

*/
QString SqLiteZaklad::vytvorCasHodinyMinutySekundy(QString vstup)
{
    qDebug()<< Q_FUNC_INFO;
    int cislo=vstup.toInt();
    int hodiny=cislo/3600;
    int minuty=(cislo%3600)/60;
    int sekundy=(cislo%3600)%60;
    vstup="";
    vstup=QString::number(hodiny)+":"+doplnNulu( minuty,2)+":"+doplnNulu(sekundy,2);
    return vstup;
}


/*!

*/
QString SqLiteZaklad::doplnNulu(int cislo,int pocetMist)
{
    //qDebug()<<"SqlPraceRopid::doplnNulu";
    QString konverze= QString::number(cislo);
    while (konverze.length()<pocetMist)
    {
        konverze="0"+konverze;
    }
    return konverze;
}


/*!

*/
bool SqLiteZaklad::jeDatumVRozsahu(QDate datum, QDate zacatek, QDate konec)
{

    qDebug()<< Q_FUNC_INFO;
    int rozdilZacatek=datum.daysTo(zacatek);
    int rozdilKonec=datum.daysTo(konec);
    qDebug()<<"pocet dni ZACATEK "<<rozdilZacatek<<" pocet dni KONEC "<<rozdilKonec;

    if ((rozdilZacatek<=0)&&(rozdilKonec>=0))
    {
        return true;
    }
    return false;
}
