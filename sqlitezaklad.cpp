#include "sqlitezaklad.h"

SqLiteZaklad::SqLiteZaklad()
{
nazevSouboru="data.sqlite";
cesta=QCoreApplication::applicationDirPath();
cestaKomplet=cesta+"/"+nazevSouboru;
}



int SqLiteZaklad::pripoj()

{
    qDebug()<< Q_FUNC_INFO;

    //this->mojeDatabaze.setHostName(adresa);
    //this->mojeDatabaze.setPort(3306);
    //this->mojeDatabaze.setHostName("127.0.0.1");




    qDebug()<<"cesta:"<<cestaKomplet;

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
    if(this->mojeDatabaze.isOpen())
    {
        return 1;
    }
    else
    {
        this->mojeDatabaze = QSqlDatabase::addDatabase("QSQLITE");
        this->mojeDatabaze.setDatabaseName(cestaKomplet);
        if(this->mojeDatabaze.open())
        {
            qDebug()<<"podarilo se pripojit k databazi ROPID";
            qDebug()<<"is driver available "<<QString::number(mojeDatabaze.isDriverAvailable("QSQLITE"));
            //qDebug()<<"je databaze otevrena "<<QString::number(mojeDatabaze.isOpen());
            //emit odesliChybovouHlasku("je databaze otevrena");
            qDebug()<<"je databaze validni "<<QString::number(mojeDatabaze.isValid());
            return 1;
        }
        else
        {
            emit odesliChybovouHlasku("DB se nepovedlo otevrit "+mojeDatabaze.lastError().text());
            qDebug()<<"DB se nepovedlo otevrit "<<mojeDatabaze.lastError();

        }
    }

    return 0;
}

bool SqLiteZaklad::existujeQueryChyba(QSqlQuery &dotaz)
{
    qDebug()<< Q_FUNC_INFO;
    QString chybaDb=dotaz.lastError().databaseText();
    QString chybaDriver=dotaz.lastError().driverText();

    if((chybaDb=="")&&(chybaDriver==""))
    {
        return 0;
    }

    emit odesliChybovouHlasku("chyba databaze: "+chybaDb+" "+chybaDriver);
    qDebug()<<"chyba databaze: "<<chybaDb<<" "<<chybaDriver;
    return 1;
}


bool SqLiteZaklad::zalozSqlTabulku(QString nazevTabulky, QVector<QString> sloupecky)
{
    qDebug()<< Q_FUNC_INFO;
    QString queryText="";

    if((nazevTabulky=="")||(sloupecky.isEmpty()))
    {
        return false;
    }

    queryText+="CREATE TABLE  "+nazevTabulky+" ("; //IF NOT EXISTS

    QString vnitrek = vektorStringuOddelovac(sloupecky,",");

    queryText+=" "+vnitrek+" ";

    queryText+=" );";
    qDebug()<<queryText;

    return spustPrikaz(queryText);



}

bool SqLiteZaklad::zrusSqlTabulku(QString nazevTabulky, QVector<QString> sloupecky)
{
    qDebug()<< Q_FUNC_INFO;
    QString queryText="";

    if((nazevTabulky=="")||(sloupecky.isEmpty()))
    {
        return false;
    }

    queryText+="DROP TABLE IF EXISTS  "+nazevTabulky+" ;";
    qDebug()<<queryText;

    return spustPrikaz(queryText);


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
    if(vstup.isEmpty())
    {
        return "";
    }
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

bool SqLiteZaklad::spustPrikaz(QString prikaz)
{
    qDebug()<< Q_FUNC_INFO;
    QSqlQuery query;
    query.exec(prikaz);


    if(existujeQueryChyba(query))
    {
        return 0;
    }


    return 1;
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

QString SqLiteZaklad::vektorStringuOddelovac(QVector<QString> vstup, QString oddelovac)
{
    QString vystup="";
  //  qDebug()<<"vektorStringu: "<<QString::number(vstup.count());
    if (vstup.isEmpty())
    {
        return "";
    }
    if (vstup.count()==1)
    {
        return vstup.first();
    }

    for(int i=0;i<vstup.count()-1;i++)
    {
        vystup+=vstup.at(i)+oddelovac;
    }
    vystup+=vstup.last();
    return vystup;
}

bool SqLiteZaklad::vlozRadekDat(QString nazevTabulky, QVector<QString> hlavicka, QVector<QString> data)
{
    QString queryText="";

    queryText+="INSERT INTO "+nazevTabulky+"( ";
    queryText+=vektorStringuOddelovac(hlavicka,",");
    queryText+=") VALUES ( ";
    queryText+=vektorStringuOddelovac(data,",");
    queryText+=" );";

   // qDebug().noquote()<<queryText;

    return spustPrikaz(queryText);


}


bool SqLiteZaklad::zahajTransakci()
{
    qDebug() <<  Q_FUNC_INFO;


    QTextStream errorStream(stderr);


    QString staryTag="";

    if(mojeDatabaze.transaction())
    {
        return 1;
    }
    else
    {
        qDebug() << "Failed to start transaction mode";
        qDebug()<<mojeDatabaze.lastError();
    }
    return 0;
}


bool SqLiteZaklad::ukonciTransakci()
{


    if(!mojeDatabaze.commit())
    {
        qDebug() << "Failed to commit";
        mojeDatabaze.rollback();
        return 0;
    }
    return 1;
}


/*
void XmlRopidImportStream::natahni(QFile &file)
{
    qDebug() <<  Q_FUNC_INFO;

    QXmlStreamReader reader(&file);
    QTextStream errorStream(stderr);


    this->databazeStart();
    QString staryTag="";

    if(ropidSQL.mojeDatabaze.transaction())
    {
        reader.readElementText(QXmlStreamReader::IncludeChildElements);


        //while inserty


        if(!ropidSQL.mojeDatabaze.commit())
        {
            qDebug() << "Failed to commit";
            ropidSQL.mojeDatabaze.rollback();
        }
    }
    else
    {
        qDebug() << "Failed to start transaction mode";
        qDebug()<<ropidSQL.mojeDatabaze.lastError();
    }

}
*/



