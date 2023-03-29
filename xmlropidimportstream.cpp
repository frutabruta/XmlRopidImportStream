#include "xmlropidimportstream.h"

#include <QtSql>
#include <QtXml>
#include <QDebug>
#include <QMainWindow>
#include <QCoreApplication>
#include <QApplication>


XmlRopidImportStream::XmlRopidImportStream()
{
    vstupniXmlSouborCesta="xml_zdroje/XML_X926_20211006_20211012";
}


int XmlRopidImportStream::databazeStart()
{
    return   ropidSQL.pripoj();
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



void XmlRopidImportStream::slotOtevriSoubor(QString cesta)
{
    emit signalNastavProgressMax(spocitejRadkySouboru(cesta));
    otevriSoubor(cesta);
}


void XmlRopidImportStream::otevriSoubor(QString cesta)
{
    qDebug() <<  Q_FUNC_INFO;
    QTime zacatek=QTime::currentTime();
    emit odesliChybovouHlasku("Zacatek importu:"+zacatek.toString() );


    QFile file(cesta);

    if (!file.open(QIODevice::ReadOnly))
    {
        emit odesliChybovouHlasku("soubor se nepovedlo otevrit");
        qDebug()<<"fail1";
        return;
    }

    bool vysledek=natahni(file);

    ropidSQL.zavriDB();
    QTime konec=QTime::currentTime();
    if(vysledek)
    {
        emit odesliChybovouHlasku("Konec importu: "+konec.toString());
        emit odesliChybovouHlasku("Doba trvání importu: "+QString::number(zacatek.secsTo(konec))) ;

    }
    else
    {
        emit odesliChybovouHlasku("Import selhal: "+konec.toString() );
        emit odesliChybovouHlasku("Doba trvání importu: "+QString::number(zacatek.secsTo(konec))) ;
    }


}


bool XmlRopidImportStream::natahni(QFile &file)
{
    qDebug() <<  Q_FUNC_INFO;

    QXmlStreamReader reader(&file);
    QTextStream errorStream(stderr);
    QVector<int> navazneSpojeObehu;

    this->databazeStart();
    QString staryTag="";

    if(ropidSQL.mojeDatabaze.transaction())
    {
        reader.readElementText(QXmlStreamReader::IncludeChildElements);

        int aktualniCisloSpoje=0;
        QXmlStreamAttributes atributyObehu;
        QXmlStreamAttributes atributyTr;
        QXmlStreamAttributes atributyTraj;
        int xCounter=0;
        int bodCounter=0;

        QVector<QString> obsah;


        while (!reader.atEnd()) {

            auto currentToken = reader.tokenString();
            QXmlStreamAttributes atributy=reader.attributes();

            if(currentToken=="StartElement")
            {
                if(staryTag=="")
                {
                    staryTag=reader.name().toString();
                }

                if((staryTag!=reader.name().toString())&&(!obsah.contains(reader.name().toString())))
                {
                    QString hlaska="Zpracovavam tag: "+reader.name().toString();
                    qDebug()<<hlaska;
                    staryTag=reader.name().toString();
                    obsah.push_back(staryTag);
                    emit odesliChybovouHlasku(hlaska);
                }

                if(reader.name()=="d")
                {
                    vlozD(atributy);
                }


                else if(reader.name()=="x")
                {
                    vlozX(atributy,xCounter,aktualniCisloSpoje);
                }


                else if(reader.name()=="t")
                {
                    vlozT(atributy);
                }
                else if(reader.name()=="dd")
                {
                    vlozDd(atributy);
                }
                else if(reader.name()=="ids")
                {
                    vlozIds(atributy);
                }
                else if(reader.name()=="m")
                {
                    vlozM(atributy);
                }

                else if(reader.name()=="g")
                {
                    vlozG(atributy);
                }
                else if(reader.name()=="z")
                {
                    vlozZ(atributy);
                }
                else if(reader.name()=="p")
                {
                    vlozP(atributy);
                }
                else if(reader.name()=="l")
                {
                    vlozL(atributy);
                }
                else if(reader.name()=="k")
                {
                    vlozK(atributy);
                }
                else if(reader.name()=="kr")
                {
                    //neni implementovano
                }
                else if(reader.name()=="vk")
                {
                    //neni implementovano
                }
                else if(reader.name()=="tr")
                {
                    atributyTr=atributy;
                    //neni implementovano
                }
                else if(reader.name()=="traj")
                {
                    atributyTraj=atributy;
                    bodCounter=0;
                    //neni implementovano
                }
                else if(reader.name()=="obl")
                {
                    //neni implementovano
                }
                else if(reader.name()=="bod")
                {
                    vlozBod(atributy,atributyTr,atributyTraj,bodCounter);
                    //neni implementovano
                }
                else if(reader.name()=="tv")
                {
                    vlozTv(atributy);
                }
                else if(reader.name()=="ty")
                {
                    vlozTy(atributy);
                }
                else if(reader.name()=="ch")
                {
                    vlozCh(atributy);
                }
                else if(reader.name()=="r")
                {
                    vlozR(atributy);
                }
                else if(reader.name()=="po")
                {
                    vlozPo(atributy);
                }

                else if(reader.name()=="o")
                {
                    atributyObehu=atributy;
                    vlozO(atributy);
                }

                else if(reader.name()=="ds")
                {
                    navazneSpojeObehu.append(seznamDlouhychSpoju(atributy));
                }

                else if(reader.name()=="s")
                {
                    aktualniCisloSpoje=vlozS(atributy);

                }



                else if(reader.name()=="v")
                {
                    //neimplementovano
                }

                else if(reader.name()=="JR_XML_EXP")
                {
                    vlozPlatnost(atributy, platnostOd, platnostDo);
                }

                else
                {
                    emit odesliChybovouHlasku("Neznámý tag: "+reader.name());
                    return false;
                }



                /*
                    else if(reader.name()=="")
                    {
                      vlozNew(atributy);
                    }

                 */

            }
            else if(currentToken=="EndElement")
            {
                if(reader.name()=="s")
                {
                    aktualniCisloSpoje=0;
                    xCounter=0;
                }
                if(reader.name()=="o")
                {
                    vlozSpPo(atributyObehu,navazneSpojeObehu);
                }
            }

            emit signalNastavProgress(reader.lineNumber());
            reader.readNext();
        }

        if(!ropidSQL.mojeDatabaze.commit())
        {
            qDebug() << "Failed to commit";
            ropidSQL.mojeDatabaze.rollback();
             emit odesliChybovouHlasku("Failed to commit");
            return false;
        }
    }
    else
    {
        qDebug() << "Failed to start transaction mode";
        qDebug()<<ropidSQL.mojeDatabaze.lastError();
        emit odesliChybovouHlasku("Failed to start transaction mode");
       return false;

    }

    return true;

}

int XmlRopidImportStream::vacuum()
{
    qDebug() <<  Q_FUNC_INFO;
    ropidSQL.pripoj();

    QString queryString = ("VACUUM;");

    qDebug().noquote()<<queryString;
    QSqlQuery query;
    query.exec(queryString);

    emit odesliChybovouHlasku("databaze zmensena");
    ropidSQL.zavriDB();
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

int XmlRopidImportStream::truncateAll()
{
    qDebug() <<  Q_FUNC_INFO;
    ropidSQL.pripoj();

    //  truncateTable("``");

    truncateTable("`t`");
    truncateTable("`sp_po`");
    truncateTable("`x_po`");
    truncateTable("`x`");
    truncateTable("`s`");
    truncateTable("`o`");
    truncateTable("`po`");
    truncateTable("`r`");
    truncateTable("`ch`");
    truncateTable("`ty`");
    truncateTable("`tv`");
    truncateTable("`k`");
    truncateTable("`l`");
    truncateTable("`p`");
    truncateTable("`z`");
    truncateTable("`m`");
    truncateTable("`ids`");
    truncateTable("`dd`");
    truncateTable("`d`");
    truncateTable("`g`");
    truncateTable("`hlavicka`");
    truncateTable("`bod`");


    emit odesliChybovouHlasku("databaze vymazana");
    ropidSQL.zavriDB();
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


int XmlRopidImportStream::vlozD(QXmlStreamAttributes atributy)
{
    // qDebug() <<  Q_FUNC_INFO;

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("ncis",atributy.value("ncis").toString(),"String"));
    polozky.push_back(inicializujPolozku("ico",atributy.value("ico").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("dic",atributy.value("dic").toString(),"String"));
    polozky.push_back(inicializujPolozku("ul",atributy.value("ul").toString(),"String"));
    polozky.push_back(inicializujPolozku("me",atributy.value("me").toString(),"String"));
    polozky.push_back(inicializujPolozku("psc",atributy.value("psc").toString(),"String"));
    polozky.push_back(inicializujPolozku("tel",atributy.value("tel").toString(),"String"));
    polozky.push_back(inicializujPolozku("em",atributy.value("em").toString(),"String"));
    QString queryString=this->slozInsert("d",polozky);

    //  QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    QSqlQuery query;
    query.exec(queryString);


    return 1;

}


int XmlRopidImportStream::vlozDd(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    QString queryString=this->slozInsert("dd",polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;
}


int XmlRopidImportStream::vlozG(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("id",atributy.value("id").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("po",atributy.value("po").toString(),"String"));
    QString queryString=this->slozInsert("g",polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;
}


int XmlRopidImportStream::vlozCh(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ch";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}

int XmlRopidImportStream::vlozIds(QXmlStreamAttributes atributy)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ids";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("tapoj",atributy.value("tapoj").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}

int XmlRopidImportStream::vlozK(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="k";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}

int XmlRopidImportStream::vlozM(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="m";

    QVector<Navrat> polozky;

    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}


int XmlRopidImportStream::vlozTv(QXmlStreamAttributes atributy)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="tv";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("dd",atributy.value("dd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("np",atributy.value("np").toString(),"Boolean"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}

int XmlRopidImportStream::vlozO(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="o";

    QVector<Navrat> polozky;

    polozky.push_back(inicializujPolozku("l",atributy.value("l").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("p",atributy.value("p").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    //přepsat pro vložení spojů jednotlivě!
    polozky.push_back(inicializujPolozku("sp",atributy.value("sp").toString(),"String"));
    polozky.push_back(inicializujPolozku("tv",atributy.value("tv").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("td",atributy.value("td").toString(),"Integer"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}



int XmlRopidImportStream::vlozPlatnost(QXmlStreamAttributes atributy, QDate &plOd, QDate &plDo)
{
    //   qDebug() <<  Q_FUNC_INFO;


    plOd=QDate::fromString(atributy.value("od").toString(),Qt::ISODate);
    plDo=QDate::fromString(atributy.value("do").toString(),Qt::ISODate);


    // QString queryString("INSERT INTO hlavicka(platnostod,platnostdo) VALUES("+ plOd.toString(Qt::ISODate)+","+plDo.toString(Qt::ISODate)+")");


    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("od",atributy.value("od").toString(),"String"));
    polozky.push_back(inicializujPolozku("do",atributy.value("do").toString(),"String"));
    polozky.push_back(inicializujPolozku("režim",atributy.value("režim").toString(),"String"));
    QString queryString=this->slozInsert("hlavicka",polozky);
    QSqlQuery query;
    query.exec(queryString);
    qDebug().noquote()<<queryString;


    QString vystup="platnost dat od "+plOd.toString(Qt::ISODate)+" do "+plDo.toString(Qt::ISODate)+" \n pocet dni:"+QString::number(plOd.daysTo(plDo)+1);
    //   QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    qDebug()<<vystup;
    emit odesliChybovouHlasku(vystup );

    return 1;
}



int XmlRopidImportStream::vlozPo(QXmlStreamAttributes atributy)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="po";

    QVector<Navrat> polozky;

    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("t",atributy.value("t").toString(),"String"));
    polozky.push_back(inicializujPolozku("zkr1",atributy.value("zkr1").toString(),"String"));
    polozky.push_back(inicializujPolozku("zkr2",atributy.value("zkr2").toString(),"String"));
    polozky.push_back(inicializujPolozku("ois",atributy.value("ois").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("zjr",atributy.value("zjr").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("csad",atributy.value("csad").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("ljr",atributy.value("ljr").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("kjr",atributy.value("kjr").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("jdf",atributy.value("jdf").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("tt",atributy.value("tt").toString(),"Boolean"));

    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("tn",atributy.value("tn").toString(),"String"));
    polozky.push_back(inicializujPolozku("nl",atributy.value("nl").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("anl",atributy.value("anl").toString(),"String"));
    polozky.push_back(inicializujPolozku("dd",atributy.value("dd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("cd",atributy.value("cd").toString(),"Integer"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    QString error=query.lastError().databaseText();
    if (error!="")
    {

        qDebug()<<query.lastError().databaseText()<<" "<<query.lastError().driverText() ;
    }

    return 1;

}


int XmlRopidImportStream::vlozR(QXmlStreamAttributes atributy)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="r";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("pop",atributy.value("pop").toString(),"String"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}


int XmlRopidImportStream::vlozTy(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ty";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("li",atributy.value("li").toString(),"String"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}


int XmlRopidImportStream::vlozSpPo(QXmlStreamAttributes atributyO,QVector<int> navazujiciSpoje)
{
    //  qDebug() <<  Q_FUNC_INFO;

    QString spoje="";
    spoje=atributyO.value("sp").toString();
    QStringList seznam=spoje.split(" ");
    QString linka=atributyO.value("l").toString();
    QString poradi=atributyO.value("p").toString();
    QString kalendar=atributyO.value("kj").toString();

    int pocetPrvku=seznam.count();
    //    qDebug()<<"pocet spoju v obehu "+atributyO.value("l")+"/"+atributyO.value("p")+" je "<<pocetPrvku;


    for (int i=0;i<pocetPrvku;i++)
    {
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("l",linka,"Integer"));
        polozky.push_back(inicializujPolozku("p",poradi,"Integer"));
        polozky.push_back(inicializujPolozku("kj",kalendar,"String"));
        //přepsat pro vložení spojů jednotlivě!
        polozky.push_back(inicializujPolozku("s",seznam.at(i),"Integer"));
        polozky.push_back(inicializujPolozku("pokrac",QString::number(navazujiciSpoje.contains(seznam.at(i).toInt())),"Boolean"));
        polozky.push_back(inicializujPolozku("ord",QString::number(i),"Integer"));
        QString queryString=this->slozInsert("sp_po",polozky);
        QSqlQuery query;
        query.exec(queryString);
    }

    return 1;
}


QVector<int> XmlRopidImportStream::seznamDlouhychSpoju(QXmlStreamAttributes atributy)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QVector<int> navazujiciSpoje;

    QString retezec= atributy.value("sp").toString();
    //   qDebug()<<"retezec dlouhych spoju "<<retezec;
    QStringList seznam= retezec.split(" ");

    if (seznam.length()>1)
    {
        seznam.removeLast();

        for (int j=0;j<seznam.length();j++)
        {
            navazujiciSpoje.append(seznam.at(j).toInt());
            //        qDebug()<<"navazujici spoj "<<seznam.at(j).toInt();
        }
    }
    else
    {
        qDebug()<<"seznam dlouhych spoju je prazdny";
    }


    return navazujiciSpoje;
}

int XmlRopidImportStream::seznamPoznamek(QXmlStreamAttributes atributy, int cisloSpoje, int poradi)
{
    //qDebug() <<  Q_FUNC_INFO;

    QString retezec= atributy.value("po").toString();
    //   qDebug()<<"retezec dlouhych spoju "<<retezec;
    QStringList seznam= retezec.split(" ");
    // qDebug()<<"vektor poznamek ma delku: "<<QString::number(seznam.length())<<" prvni element je:"<<seznam.first()<<":";
    QString nazevElementu="x_po";

    if(seznam.first()=="")
    {
        //  qDebug()<<"prvni poznamka je prazdna";
        return 0;
    }
    if (seznam.length()>0)
    {

        for (int j=0;j<seznam.length();j++)
        {
            QVector<Navrat> polozky;
            polozky.push_back(inicializujPolozku("xorder",QString::number(poradi),"Integer"));
            polozky.push_back(inicializujPolozku("s",QString::number(cisloSpoje),"Integer"));
            polozky.push_back(inicializujPolozku("po",seznam.at(j),"Integer"));
            QString queryString=this->slozInsert(nazevElementu,polozky);
            QSqlQuery query;
            // qDebug().noquote()<<queryString;
            query.exec(queryString);
        }
    }
    else
    {
        //qDebug()<<"seznam poznamek je prazdny";
        return 0;
    }


    return 1;
}


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


int XmlRopidImportStream::vlozL(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="l";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("d",atributy.value("d").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("lc",atributy.value("lc").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("tl",atributy.value("tl").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("kup",atributy.value("kup").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ids",atributy.value("ids").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("noc",atributy.value("noc").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("aois",atributy.value("aois").toString(),"String"));
    polozky.push_back(inicializujPolozku("a",atributy.value("a").toString(),"String"));
    polozky.push_back(inicializujPolozku("kli",atributy.value("kli").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("cids",atributy.value("cids").toString(),"String"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}


int XmlRopidImportStream::vlozP(QXmlStreamAttributes atributy)
{
    // qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="p";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("d",atributy.value("d").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("dd",atributy.value("dd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("u",atributy.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("mail",atributy.value("mail").toString(),"String"));
    polozky.push_back(inicializujPolozku("telv",atributy.value("telg").toString(),"String"));
    polozky.push_back(inicializujPolozku("telg",atributy.value("telv").toString(),"String"));
    polozky.push_back(inicializujPolozku("telz",atributy.value("telz").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;
}


int XmlRopidImportStream::vlozZ(QXmlStreamAttributes atributy)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="z";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("u",atributy.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("pop",atributy.value("pop").toString(),"String"));
    polozky.push_back(inicializujPolozku("tu",atributy.value("tu").toString(),"String"));
    polozky.push_back(inicializujPolozku("cis",atributy.value("cis").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ois",atributy.value("ois").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("co",atributy.value("co").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("no",atributy.value("no").toString(),"String"));
    polozky.push_back(inicializujPolozku("spz",atributy.value("spz").toString(),"String"));
    polozky.push_back(inicializujPolozku("ids",atributy.value("ids").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ids2",atributy.value("ids2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ids3",atributy.value("ids3").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("tp",atributy.value("tp").toString(),"String"));
    polozky.push_back(inicializujPolozku("tp2",atributy.value("tp2").toString(),"String"));
    polozky.push_back(inicializujPolozku("tp3",atributy.value("tp3").toString(),"String"));
    polozky.push_back(inicializujPolozku("sx",atributy.value("sx").toString(),"String"));
    polozky.push_back(inicializujPolozku("sy",atributy.value("sx").toString(),"String"));
    polozky.push_back(inicializujPolozku("lat",atributy.value("lat").toString(),"String"));
    polozky.push_back(inicializujPolozku("lng",atributy.value("lng").toString(),"String"));
    polozky.push_back(inicializujPolozku("sta",atributy.value("sta").toString(),"String"));
    polozky.push_back(inicializujPolozku("m",atributy.value("m").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("bbn",atributy.value("bbn").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("kidos",atributy.value("kidos").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("st",atributy.value("st").toString(),"String"));
    polozky.push_back(inicializujPolozku("xA",atributy.value("xA").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xB",atributy.value("xB").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xC",atributy.value("xC").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xVla",atributy.value("xVla").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLod",atributy.value("xLod").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLet",atributy.value("xLed").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("rdisp",atributy.value("rdisp").toString(),"Integer"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}


int XmlRopidImportStream::vlozS(QXmlStreamAttributes atributy)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="s";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("s",atributy.value("s").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("id",atributy.value("id").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("l",atributy.value("l").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("p",atributy.value("p").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("dd",atributy.value("dd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("pr",atributy.value("pr").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("d",atributy.value("d").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("tv",atributy.value("tv").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("ty",atributy.value("ty").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ch",atributy.value("ch").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ids",atributy.value("ids").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("vy",atributy.value("vy").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("man",atributy.value("man").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("neve",atributy.value("neve").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("ns",atributy.value("ns").toString(),"Integer"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);




    return atributy.value("s").toInt();
}


int XmlRopidImportStream::vlozT(QXmlStreamAttributes atributy)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="t";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("u",atributy.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("ois",atributy.value("ois").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("cis",atributy.value("cis").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("nza",atributy.value("nza").toString(),"String"));
    polozky.push_back(inicializujPolozku("ri",atributy.value("ri").toString(),"String"));
    polozky.push_back(inicializujPolozku("ji",atributy.value("ji").toString(),"String"));
    polozky.push_back(inicializujPolozku("vtm",atributy.value("vtm").toString(),"String"));
    polozky.push_back(inicializujPolozku("vtn",atributy.value("vtn").toString(),"String"));
    polozky.push_back(inicializujPolozku("btm",atributy.value("btm").toString(),"String"));
    polozky.push_back(inicializujPolozku("btn",atributy.value("btn").toString(),"String"));
    polozky.push_back(inicializujPolozku("ctm",atributy.value("ctm").toString(),"String"));
    polozky.push_back(inicializujPolozku("ctn",atributy.value("ctn").toString(),"String"));
    polozky.push_back(inicializujPolozku("lcdm",atributy.value("lcdm").toString(),"String"));
    polozky.push_back(inicializujPolozku("lcdn",atributy.value("lcdn").toString(),"String"));
    polozky.push_back(inicializujPolozku("hl",atributy.value("hl").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}


int XmlRopidImportStream::vlozX(QXmlStreamAttributes atributy, int &counter, int cisloSpoje)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="x";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("s_id",QString::number(cisloSpoje),"Integer"));
    polozky.push_back(inicializujPolozku("u",atributy.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("p",atributy.value("p").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("o",atributy.value("o").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("t",atributy.value("t").toString(),"String"));
    polozky.push_back(inicializujPolozku("ty",atributy.value("ty").toString(),"Integer"));


    polozky.push_back(inicializujPolozku("ces",atributy.value("ces").toString(),"BooleanInv"));
   // qDebug().noquote()<<"CES: "<<atributy.value("ces").toString();

    polozky.push_back(inicializujPolozku("po",atributy.value("po").toString(),"String"));
    polozky.push_back(inicializujPolozku("zn",atributy.value("zn").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("na",atributy.value("na").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("vyst",atributy.value("vyst").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("nast",atributy.value("nast").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xA",atributy.value("xA").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xB",atributy.value("xB").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xC",atributy.value("xC").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xD",atributy.value("xC").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xVla",atributy.value("xVla").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLod",atributy.value("xLod").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLet",atributy.value("xLed").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xorder",QString::number(counter),"Integer"));
    polozky.push_back(inicializujPolozku("zsol",atributy.value("zsol").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("s1",atributy.value("s1").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("s2",atributy.value("s2").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("var",atributy.value("var").toString(),"Integer"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);
    seznamPoznamek(atributy,cisloSpoje,counter );
    counter++;

    return 1;
}


int XmlRopidImportStream::vlozBod(QXmlStreamAttributes atributy, QXmlStreamAttributes atributyTr, QXmlStreamAttributes atributyTraj, int &counter)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="bod";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("zvd",atributyTr.value("zvd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("u1",atributyTr.value("u1").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z1",atributyTr.value("z1").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("u2",atributyTr.value("u2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z2",atributyTr.value("z2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("var",atributyTr.value("var").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributyTr.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("c",atributyTraj.value("c").toString(),"Integer"));


    polozky.push_back(inicializujPolozku("x",atributy.value("X").toString(),"String"));
    polozky.push_back(inicializujPolozku("y",atributy.value("Y").toString(),"String"));
     polozky.push_back(inicializujPolozku("poradi",QString::number(counter),"Integer"));


    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);
    //qDebug().noquote()<<queryString;
    counter++;

    return 1;
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
