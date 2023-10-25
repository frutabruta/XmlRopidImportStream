#include "xmlropidimportstream.h"




XmlRopidImportStream::XmlRopidImportStream()
{
    vstupniXmlSouborCesta="xml_zdroje/XML_X926_20211006_20211012";
}


int XmlRopidImportStream::databazeStart()
{
    return   sqLiteZaklad.pripoj();
}


XmlRopidImportStream::Navrat XmlRopidImportStream::inicializujPolozku(QString nazevSloupce,QString obsah,QString typ)
{
    Navrat navrat;
    navrat.typ=typ;
    navrat.obsah=obsah;
    navrat.nazevSloupce=nazevSloupce;

    if (navrat.typ=="String")
    {
        navrat.obsah=overString(obsah);
    }
    if (navrat.typ=="Integer")
    {
        navrat.obsah=overInteger(obsah);
    }
    if (navrat.typ=="Boolean")
    {
        navrat.obsah=overBoolean(obsah);
    }
    if (navrat.typ=="BooleanInv")
    {
        navrat.obsah=overBooleanInv(obsah);
    }

    return navrat;
}

int XmlRopidImportStream::vacuum()
{
    qDebug() <<  Q_FUNC_INFO;
    sqLiteZaklad.pripoj();

    QString queryString = ("VACUUM;");

    qDebug().noquote()<<queryString;
    QSqlQuery query;
    query.exec(queryString);

    emit odesliChybovouHlasku("databaze zmensena");
    sqLiteZaklad.zavriDB();
    return 1;

}




int XmlRopidImportStream::truncateTable(QString tabulka)
{
    qDebug() <<  Q_FUNC_INFO;
    QString queryString = ("DELETE FROM ");
    queryString+=tabulka;
    qDebug().noquote()<<queryString;
    QSqlQuery query;
    query.exec(queryString);
    return 1;
}



//vzorovy vloz new
/*
int XmlRopidImportStream::vlozNew(QXmlStreamAttributes atributy)
{
    qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="";

    QVector<navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}

*/

QString XmlRopidImportStream::slozInsert(QString nazevTabulky, QVector<Navrat> seznam)
{
    QString prikaz="";
    QString zacatek="INSERT INTO "+nazevTabulky+" (";
    QString hodnoty="";
    QString konec="  );";
    for(int i=0;i<(seznam.length());i++)
    {
        zacatek+=seznam[i].nazevSloupce;
        hodnoty+=seznam[i].obsah;
        if (i!= seznam.length()-1 )
        {
            zacatek+=" , ";
            hodnoty+=" , ";
        }
    }

    QString prostredek=") VALUES( ";
    prikaz=zacatek+prostredek+hodnoty+konec;
    return prikaz;
}



QString XmlRopidImportStream::overBoolean(QString vstup)
{
    if (vstup=="")
    {
        vstup="false";
    }


    return vstup;
}

QString XmlRopidImportStream::overBooleanInv(QString vstup)
{
    if (vstup=="")
    {
        vstup="true";
    }


    return vstup;
}

QString XmlRopidImportStream::overInteger(QString vstup)
{
    if (vstup=="")
    {
        vstup="NULL";
    }

    return vstup;
}

QString XmlRopidImportStream::overString(QString vstup)
{
    if (vstup=="")
    {
        vstup="NULL";
    }
    else
    {
        vstup.replace("\"","\"\"");
        vstup="\""+vstup+"\"";
    }

    return vstup;
}

QString XmlRopidImportStream::vytvorCas(QString vstup)
{
    int cislo=vstup.toInt();
    int hodiny=cislo/3600;
    int minuty=(cislo%3600)/60;
    int sekundy=(cislo%3600)%60;
    vstup="";
    vstup=QString::number(hodiny)+""+QString::number(minuty)+""+QString::number(sekundy);
    return vstup;
}



int XmlRopidImportStream::spocitejRadkySouboru(QString fileName)
{
    qDebug()<<Q_FUNC_INFO;

    // zdroj: https://stackoverflow.com/questions/5444959/read-a-text-file-line-by-line-in-qt
    QFile inputFile(fileName);

    int counter=0;

    if (inputFile.open(QIODevice::ReadOnly))
    {

        QTextStream in_c(&inputFile);
        while (!in_c.atEnd())
        {
            in_c.readLine();
            counter++;
        }
        inputFile.close();
    }

    qDebug()<<"soubor ma "<<counter<<" radku";
    qDebug()<<"konec soubornaRadky";
    return counter;
}


void XmlRopidImportStream::vlozPoleAtributu(QString nazevTabulky, QMap<QString,QString> atributy)
{
    QVector<QString> nazvy;
    QVector<QString> hodnoty;

    QVector<Navrat> polozky;

    for(int i=0;i<atributy.count();i++)
    {
        QString klic=atributy.keys().at(i);
        nazvy.push_back(klic);
        hodnoty.push_back(atributy[klic]);

        polozky.push_back(inicializujPolozku(klic,atributy.value(klic),"String"));

    }
    QString queryString=this->slozInsert(nazevTabulky,polozky);

    QSqlQuery query;
    // qDebug().noquote()<<queryString ;//<<" pocet polozek"<<atributy.count();
    query.exec(queryString);
    sqLiteZaklad.existujeQueryChyba(query);


}

void XmlRopidImportStream::QMapAppend(QMap<QString,QString> &puvodni, QMap<QString,QString> pridany)
{
    for(int i=0;i<pridany.count();i++)
    {
        puvodni.insert(pridany.keys().at(i),pridany.values().at(i));
    }


}



