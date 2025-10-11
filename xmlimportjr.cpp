#include "xmlimportjr.h"
/*
#include <QtSql>
#include <QtXml>
#include <QDebug>
#include <QMainWindow>
#include <QCoreApplication>
#include <QApplication>
*/

XmlImportJr::XmlImportJr()
{
    vstupniXmlSouborCesta="xml_zdroje/XML_X926_20211006_20211012";
}








void XmlImportJr::slotOtevriSoubor(QString cesta)
{
    emit signalNastavProgressMax(spocitejRadkySouboru(cesta));
    otevriSoubor(cesta);
}


void XmlImportJr::otevriSoubor(QString cesta)
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

    sqLiteZaklad.zavriDB();
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


bool XmlImportJr::natahni(QFile &file)
{
    qDebug() <<  Q_FUNC_INFO;

    QXmlStreamReader reader(&file);
    QTextStream errorStream(stderr);
    QVector<int> navazneSpojeObehu;

    this->databazeStart();
    QString staryTag="";

    if(sqLiteZaklad.mojeDatabaze.transaction())
    {
        reader.readElementText(QXmlStreamReader::IncludeChildElements);

        int aktualniCisloSpoje=0;
        QXmlStreamAttributes atributyObehu;
        QXmlStreamAttributes atributyTr;
        QXmlStreamAttributes atributyTraj;
        QXmlStreamAttributes atributyZ;
        QXmlStreamAttributes atributyPol;
        int xCounter=0;
        int bodCounter=0;
        int bodCounterPoly=0;

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
                    QString hlaska="Zpracovávám tag: "+reader.name().toString();
                    qDebug()<<hlaska;
                    staryTag=reader.name().toString();
                    obsah.push_back(staryTag);
                    emit odesliChybovouHlasku(hlaska);
                }

                if(reader.name()==QString("d"))
                {
                    vlozD(atributy);
                }


                else if(reader.name()==QString("x"))
                {
                    vlozX(atributy,xCounter,aktualniCisloSpoje);
                }


                else if(reader.name()==QString("t"))
                {
                    vlozT(atributy);
                }
                else if(reader.name()==QString("dd"))
                {
                    vlozDd(atributy);
                }
                else if(reader.name()==QString("ids"))
                {
                    vlozIds(atributy);
                }
                else if(reader.name()==QString("m"))
                {
                    vlozM(atributy);
                }

                else if(reader.name()==QString("g"))
                {
                    vlozG(atributy);
                }
                else if(reader.name()==QString("z"))
                {
                    atributyZ=atributy;
                    vlozZ(atributy);

                }              
                else if(reader.name()==QString("p"))
                {
                    vlozP(atributy);
                }
                else if(reader.name()==QString("pol"))
                {
                    atributyPol=atributy;
                    bodCounterPoly=0;
                }
                else if(reader.name()==QString("l"))
                {
                    vlozL(atributy);
                }
                else if(reader.name()==QString("k"))
                {
                    vlozK(atributy);
                }
                else if(reader.name()==QString("kr"))
                {
                    //neni implementovano
                }
                else if(reader.name()==QString("vk"))
                {
                    //neni implementovano
                }
                else if(reader.name()==QString("tr"))
                {
                    atributyTr=atributy;
                }
                else if(reader.name()==QString("traj"))
                {
                    atributyTraj=atributy;
                    bodCounter=0;             
                }
                else if(reader.name()==QString("obl"))
                {
                    vlozObl(atributy);
                }
                else if(reader.name()==QString("bod"))
                {
                    if(atributyTr.isEmpty()||atributyTraj.isEmpty())
                    {
                        if(atributyPol.isEmpty()||atributyZ.isEmpty())
                        {
                            qDebug()<<"empty pol or zast";
                        }
                        else
                        {
                            vlozBodPol(atributy,atributyZ,atributyPol,bodCounterPoly);
                        }

                    }
                    else
                    {
                        vlozBodTraj(atributy,atributyTr,atributyTraj,bodCounter);
                    }

                }
                else if(reader.name()==QString("wgs"))
                {
                    vlozWgs(atributy,atributyTr,atributyTraj,bodCounter);
                }
                else if(reader.name()==QString("tv"))
                {
                    vlozTv(atributy);
                }
                else if(reader.name()==QString("ty"))
                {
                    vlozTy(atributy);
                }
                else if(reader.name()==QString("ch"))
                {
                    vlozCh(atributy);
                }
                else if(reader.name()==QString("r"))
                {
                    vlozR(atributy);
                }
                else if(reader.name()==QString("po"))
                {
                    vlozPo(atributy);
                }

                else if(reader.name()==QString("o"))
                {
                    atributyObehu=atributy;
                    vlozO(atributy);
                }

                else if(reader.name()==QString("ds"))
                {
                    navazneSpojeObehu.append(seznamDlouhychSpoju(atributy));
                }

                else if(reader.name()==QString("s"))
                {
                    aktualniCisloSpoje=vlozS(atributy);

                }



                else if(reader.name()==QString("v"))
                {
                    //neimplementovano
                }

                else if(reader.name()==QString("JR_XML_EXP"))
                {
                    vlozPlatnost(atributy, platnostOd, platnostDo,file.fileName());
                }

                else
                {
                    emit odesliChybovouHlasku("Neznámý tag: "+reader.name().toString());
                    //return false;
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
                if(reader.name()==QString("s"))
                {
                    aktualniCisloSpoje=0;
                    xCounter=0;
                }
                else if(reader.name()==QString("o"))
                {
                    vlozSpPo(atributyObehu,navazneSpojeObehu);
                }
                else if(reader.name()==QString("z"))
                {
                    atributyZ.clear();
                }
                else if(reader.name()==QString("pol"))
                {
                    atributyPol.clear();
                }
                else if(reader.name()==QString("tr"))
                {
                    atributyTr.clear();
                }
                else if(reader.name()==QString("traj"))
                {
                    atributyTraj.clear();
                }
            }

            emit signalNastavProgress(reader.lineNumber());
            reader.readNext();
        }

        if(!sqLiteZaklad.mojeDatabaze.commit())
        {
            qDebug() << "Failed to commit";
            sqLiteZaklad.mojeDatabaze.rollback();
            emit odesliChybovouHlasku("Failed to commit");
            return false;
        }
    }
    else
    {
        qDebug() << "Failed to start transaction mode";
        qDebug()<<sqLiteZaklad.mojeDatabaze.lastError();
        emit odesliChybovouHlasku("Failed to start transaction mode");
        return false;

    }

    return true;

}






int XmlImportJr::truncateAll()
{
    qDebug() <<  Q_FUNC_INFO;


    //  truncateTable("``");

    truncateTimetables();
    truncateApc();
    truncateTasks();

    emit odesliChybovouHlasku("all data deleted");

    return 1;


}


int XmlImportJr::truncateApc()
{
    qDebug() <<  Q_FUNC_INFO;
    sqLiteZaklad.pripoj();

    //  truncateTable("``");

    truncateTable("`apc_dpp`");
    truncateTable("`apc_opv`");

    emit odesliChybovouHlasku("apc data deleted");
    sqLiteZaklad.zavriDB();
    return 1;


}


int XmlImportJr::truncateTimetables()
{
    qDebug() <<  Q_FUNC_INFO;
    sqLiteZaklad.pripoj();

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
    truncateTable("`obl`");
    truncateTable("`hlavicka`");
    truncateTable("`bod`");
    truncateTable("`bod_polygon`");

    emit odesliChybovouHlasku("timetables data deleted");
    sqLiteZaklad.zavriDB();
    return 1;
}



int XmlImportJr::truncateTasks()
{
    qDebug() <<  Q_FUNC_INFO;
    sqLiteZaklad.pripoj();

    //  truncateTable("``");

    truncateTable("`ukoly_zaznam`");
    truncateTable("`ukoly_zaznam_dopravce`");

    emit odesliChybovouHlasku("tasks data deleted");
    sqLiteZaklad.zavriDB();
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
    QSqlQuery query(queryString,sqLiteZaklad.mojeDatabaze);

    return 1;

}

*/




/*

QString XmlImportJr::vytvorCas(QString vstup)
{
    int cislo=vstup.toInt();
    int hodiny=cislo/3600;
    int minuty=(cislo%3600)/60;
    int sekundy=(cislo%3600)%60;
    vstup="";
    vstup=QString::number(hodiny)+""+QString::number(minuty)+""+QString::number(sekundy);
    return vstup;
}
*/

int XmlImportJr::vlozD(QXmlStreamAttributes atributy)
{
    // qDebug() <<  Q_FUNC_INFO;

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("ncis",atributy.value("ncis").toString(),"String"));
    polozky.push_back(inicializujPolozku("ico",atributy.value("ico").toString(),"String"));
    polozky.push_back(inicializujPolozku("dic",atributy.value("dic").toString(),"String"));
    polozky.push_back(inicializujPolozku("ul",atributy.value("ul").toString(),"String"));
    polozky.push_back(inicializujPolozku("me",atributy.value("me").toString(),"String"));
    polozky.push_back(inicializujPolozku("psc",atributy.value("psc").toString(),"String"));
    polozky.push_back(inicializujPolozku("tel",atributy.value("tel").toString(),"String"));
    polozky.push_back(inicializujPolozku("teld",atributy.value("teld").toString(),"String"));
    polozky.push_back(inicializujPolozku("teli",atributy.value("teli").toString(),"String"));
    polozky.push_back(inicializujPolozku("em",atributy.value("em").toString(),"String"));
    QString queryString=this->slozInsert("d",polozky);

    //  QSqlQuery query(queryString,sqLiteZaklad.mojeDatabaze);

    QSqlQuery query;
    query.exec(queryString);


    return 1;

}


int XmlImportJr::vlozDd(QXmlStreamAttributes atributy)
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


int XmlImportJr::vlozG(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("id",atributy.value("id").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("po",atributy.value("po").toString(),"String"));
    polozky.push_back(inicializujPolozku("zvd",atributy.value("zvd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("pd",atributy.value("pd").toString(),"String"));
    QString queryString=this->slozInsert("g",polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;
}


int XmlImportJr::vlozCh(QXmlStreamAttributes atributy)
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

int XmlImportJr::vlozIds(QXmlStreamAttributes atributy)
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

int XmlImportJr::vlozK(QXmlStreamAttributes atributy)
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

int XmlImportJr::vlozM(QXmlStreamAttributes atributy)
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


int XmlImportJr::vlozTv(QXmlStreamAttributes atributy)
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

int XmlImportJr::vlozO(QXmlStreamAttributes atributy)
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
    polozky.push_back(inicializujPolozku("vyb",atributy.value("vyb").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}


int XmlImportJr::vlozObl(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="obl";

    QVector<Navrat> polozky;

    polozky.push_back(inicializujPolozku("id",atributy.value("id").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));


    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}



int XmlImportJr::vlozPlatnost(QXmlStreamAttributes atributy, QDate &plOd, QDate &plDo, QString fileName)
{
    //   qDebug() <<  Q_FUNC_INFO;


    plOd=QDate::fromString(atributy.value("od").toString(),Qt::ISODate);
    plDo=QDate::fromString(atributy.value("do").toString(),Qt::ISODate);


    // QString queryString("INSERT INTO hlavicka(platnostod,platnostdo) VALUES("+ plOd.toString(Qt::ISODate)+","+plDo.toString(Qt::ISODate)+")");


    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("od",atributy.value("od").toString(),"String"));
    polozky.push_back(inicializujPolozku("do",atributy.value("do").toString(),"String"));
    polozky.push_back(inicializujPolozku("režim",atributy.value("režim").toString(),"String"));
    polozky.push_back(inicializujPolozku("exp",atributy.value("exp").toString(),"String"));
    polozky.push_back(inicializujPolozku("ver",atributy.value("ver").toString(),"String"));
    polozky.push_back(inicializujPolozku("filepath",fileName,"String"));
    QString queryString=this->slozInsert("hlavicka",polozky);
    QSqlQuery query;
    query.exec(queryString);
    qDebug().noquote()<<queryString;


    QString vystup="platnost dat od "+plOd.toString(Qt::ISODate)+" do "+plDo.toString(Qt::ISODate)+" \n pocet dni:"+QString::number(plOd.daysTo(plDo)+1);
    //   QSqlQuery query(queryString,sqLiteZaklad.mojeDatabaze);
    qDebug()<<vystup;
    emit odesliChybovouHlasku(vystup );

    return 1;
}



int XmlImportJr::vlozPo(QXmlStreamAttributes atributy)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="po";

    QVector<Navrat> polozky;

    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("t",atributy.value("t").toString(),"String"));
    polozky.push_back(inicializujPolozku("zkr1",atributy.value("zkr1").toString(),"String"));
    polozky.push_back(inicializujPolozku("zkr2",atributy.value("zkr2").toString(),"String"));
    polozky.push_back(inicializujPolozku("zkr3",atributy.value("zkr3").toString(),"String"));
    polozky.push_back(inicializujPolozku("ois",atributy.value("ois").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("zjr",atributy.value("zjr").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("csad",atributy.value("csad").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("vjr",atributy.value("vjr").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("ljr",atributy.value("ljr").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("kjr",atributy.value("kjr").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("jdf",atributy.value("jdf").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("tt",atributy.value("tt").toString(),"Boolean"));    
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("tn",atributy.value("tn").toString(),"String"));
    polozky.push_back(inicializujPolozku("u",atributy.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("u2",atributy.value("u2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z2",atributy.value("z2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("nl",atributy.value("nl").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("anl",atributy.value("anl").toString(),"String"));
    polozky.push_back(inicializujPolozku("cd",atributy.value("cd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("usm",atributy.value("usm").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("zsm",atributy.value("zsm").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("dd",atributy.value("dd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("mind",atributy.value("mind").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("thls",atributy.value("thls").toString(),"Boolean"));   //added in 1.41
    polozky.push_back(inicializujPolozku("tpan",atributy.value("tpan").toString(),"String"));   //added in 1.41
    polozky.push_back(inicializujPolozku("kan",atributy.value("kan").toString(),"String"));   //added in 1.41
    polozky.push_back(inicializujPolozku("akce",atributy.value("akce").toString(),"String"));   //added in 1.41
    polozky.push_back(inicializujPolozku("nahr",atributy.value("nahr").toString(),"String"));    //added in 1.41



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


int XmlImportJr::vlozR(QXmlStreamAttributes atributy)
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


int XmlImportJr::vlozTy(QXmlStreamAttributes atributy)
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


int XmlImportJr::vlozSpPo(QXmlStreamAttributes atributyO,QVector<int> navazujiciSpoje)
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


QVector<int> XmlImportJr::seznamDlouhychSpoju(QXmlStreamAttributes atributy)
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

int XmlImportJr::seznamPoznamek(QXmlStreamAttributes atributy, int cisloSpoje, int poradi)
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





int XmlImportJr::vlozL(QXmlStreamAttributes atributy)
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


int XmlImportJr::vlozP(QXmlStreamAttributes atributy)
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
    polozky.push_back(inicializujPolozku("cis",atributy.value("cis").toString(),"Integer")); //added in 1.37
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;
}


int XmlImportJr::vlozZ(QXmlStreamAttributes atributy)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="z";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("u",atributy.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("n2",atributy.value("n2").toString(),"String"));
    polozky.push_back(inicializujPolozku("n3",atributy.value("n3").toString(),"String"));
    polozky.push_back(inicializujPolozku("n4",atributy.value("n4").toString(),"String"));
    polozky.push_back(inicializujPolozku("n5",atributy.value("n5").toString(),"String"));
    polozky.push_back(inicializujPolozku("n6",atributy.value("n6").toString(),"String"));
    polozky.push_back(inicializujPolozku("n7",atributy.value("n7").toString(),"String"));
    polozky.push_back(inicializujPolozku("n8",atributy.value("n8").toString(),"String"));
    polozky.push_back(inicializujPolozku("pop",atributy.value("pop").toString(),"String"));
    polozky.push_back(inicializujPolozku("tu",atributy.value("tu").toString(),"String"));
    polozky.push_back(inicializujPolozku("cis",atributy.value("cis").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ois",atributy.value("ois").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ois2",atributy.value("ois2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("co",atributy.value("co").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("no",atributy.value("no").toString(),"String"));
    polozky.push_back(inicializujPolozku("cco",atributy.value("cco").toString(),"String"));
    polozky.push_back(inicializujPolozku("nco",atributy.value("nco").toString(),"String"));
    polozky.push_back(inicializujPolozku("spz",atributy.value("spz").toString(),"String"));
    polozky.push_back(inicializujPolozku("ids",atributy.value("ids").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ids2",atributy.value("ids2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ids3",atributy.value("ids3").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("tp",atributy.value("tp").toString(),"String"));
    polozky.push_back(inicializujPolozku("tp2",atributy.value("tp2").toString(),"String"));
    polozky.push_back(inicializujPolozku("tp3",atributy.value("tp3").toString(),"String"));
    polozky.push_back(inicializujPolozku("sx",atributy.value("sx").toString(),"String"));
    polozky.push_back(inicializujPolozku("sy",atributy.value("sy").toString(),"String"));
    polozky.push_back(inicializujPolozku("lat",atributy.value("lat").toString(),"String"));
    polozky.push_back(inicializujPolozku("lng",atributy.value("lng").toString(),"String"));
    polozky.push_back(inicializujPolozku("sta",atributy.value("sta").toString(),"String"));
    polozky.push_back(inicializujPolozku("m",atributy.value("m").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kmen",atributy.value("kmen").toString(),"Integer")); //added in 1.37
    polozky.push_back(inicializujPolozku("ve",atributy.value("ve").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("bbn",atributy.value("bbn").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("kidos",atributy.value("kidos").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("st",atributy.value("st").toString(),"String"));
    polozky.push_back(inicializujPolozku("xA",atributy.value("xA").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xB",atributy.value("xB").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xC",atributy.value("xC").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xD",atributy.value("xD").toString(),"Boolean")); //added in 1.38
    polozky.push_back(inicializujPolozku("xVla",atributy.value("xVla").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLod",atributy.value("xLod").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLet",atributy.value("xLed").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("rdisp",atributy.value("rdisp").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("neprep",atributy.value("neprep").toString(),"Boolean"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);
    //  qDebug().noquote()<<queryString;
    return 1;

}


int XmlImportJr::vlozS(QXmlStreamAttributes atributy)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="s";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("s",atributy.value("s").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("id",atributy.value("id").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("zvd",atributy.value("zvd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("l",atributy.value("l").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("p",atributy.value("p").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("sm",atributy.value("sm").toString(),"BooleanInv"));
    polozky.push_back(inicializujPolozku("dd",atributy.value("dd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("pr",atributy.value("pr").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("d",atributy.value("d").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("tv",atributy.value("tv").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("ty",atributy.value("ty").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ch",atributy.value("ch").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("np",atributy.value("np").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("ids",atributy.value("ids").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("jk",atributy.value("jk").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("vy",atributy.value("vy").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("sp1",atributy.value("sp1").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("spN",atributy.value("spN").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("doh",atributy.value("doh").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("pos",atributy.value("pos").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("man",atributy.value("man").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("skol",atributy.value("skol").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("lc",atributy.value("lc").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("neve",atributy.value("neve").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("obl",atributy.value("obl").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ns",atributy.value("ns").toString(),"Integer"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);




    return atributy.value("s").toInt();
}


int XmlImportJr::vlozT(QXmlStreamAttributes atributy)
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
    polozky.push_back(inicializujPolozku("vtmnoc",atributy.value("vtmnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("vtnnoc",atributy.value("vtnnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("btm",atributy.value("btm").toString(),"String"));
    polozky.push_back(inicializujPolozku("btn",atributy.value("btn").toString(),"String"));
    polozky.push_back(inicializujPolozku("btmnoc",atributy.value("btmnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("btnnoc",atributy.value("btnnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("ctm",atributy.value("ctm").toString(),"String"));
    polozky.push_back(inicializujPolozku("ctn",atributy.value("ctn").toString(),"String"));
    polozky.push_back(inicializujPolozku("ctmnoc",atributy.value("ctmnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("ctnnoc",atributy.value("ctnnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("ztm",atributy.value("ztm").toString(),"String")); //added in ???
    polozky.push_back(inicializujPolozku("ztn",atributy.value("ztn").toString(),"String")); //added in ???
    polozky.push_back(inicializujPolozku("ztmnoc",atributy.value("ztmnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("ztnnoc",atributy.value("ztnnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("lcdm",atributy.value("lcdm").toString(),"String"));
    polozky.push_back(inicializujPolozku("lcdn",atributy.value("lcdn").toString(),"String"));
    polozky.push_back(inicializujPolozku("lcdmnoc",atributy.value("lcdmnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("lcdnnoc",atributy.value("lcdnnoc").toString(),"String")); //added in 1.41
    polozky.push_back(inicializujPolozku("hl",atributy.value("hl").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("nf",atributy.value("nf").toString(),"String"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);

    return 1;

}


int XmlImportJr::vlozX(QXmlStreamAttributes atributy, int &counter, int cisloSpoje)
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
    polozky.push_back(inicializujPolozku("neozn",atributy.value("neozn").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("zast",atributy.value("zast").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("vyst",atributy.value("vyst").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("nast",atributy.value("nast").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("na",atributy.value("na").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("poj",atributy.value("poj").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("bp",atributy.value("bp").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("zpl",atributy.value("zpl").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("kpl",atributy.value("kpl").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("s",atributy.value("s").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("dm",atributy.value("dm").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("s1",atributy.value("s1").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("s2",atributy.value("s2").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("oc",atributy.value("oc").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xA",atributy.value("xA").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xB",atributy.value("xB").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xC",atributy.value("xC").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xD",atributy.value("xD").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xS",atributy.value("xS").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xTra",atributy.value("xTra").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xBus",atributy.value("xBus").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xTro",atributy.value("xTro").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xVla",atributy.value("xVla").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLod",atributy.value("xLod").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLet",atributy.value("xLet").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xLan",atributy.value("xLan").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("xorder",QString::number(counter),"Integer"));
    polozky.push_back(inicializujPolozku("zsol",atributy.value("zsol").toString(),"Boolean"));
    polozky.push_back(inicializujPolozku("icls",atributy.value("icls").toString(),"Integer"));

    polozky.push_back(inicializujPolozku("var",atributy.value("var").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("nev",atributy.value("nev").toString(),"Boolean"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);
    seznamPoznamek(atributy,cisloSpoje,counter );
    counter++;

    return 1;
}


int XmlImportJr::vlozBodTraj(QXmlStreamAttributes atributy, QXmlStreamAttributes atributyTr, QXmlStreamAttributes atributyTraj, int &counter)
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

int XmlImportJr::vlozBodPol(QXmlStreamAttributes atributy, QXmlStreamAttributes atributyZast, QXmlStreamAttributes atributyPol, int &counter)
{
    //qDebug() <<  Q_FUNC_INFO;
    //added in 1.41
    QString nazevElementu="bod_polygon";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("u",atributyZast.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributyZast.value("z").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributyZast.value("kj").toString(),"String"));



    polozky.push_back(inicializujPolozku("c",atributyPol.value("c").toString(),"Integer"));


    polozky.push_back(inicializujPolozku("lat",atributy.value("lat").toString(),"String"));
    polozky.push_back(inicializujPolozku("lon",atributy.value("lon").toString(),"String"));

    polozky.push_back(inicializujPolozku("poradi",QString::number(counter),"Integer"));


    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);
    //qDebug().noquote()<<queryString;
    counter++;

    return 1;
}


int XmlImportJr::vlozWgs(QXmlStreamAttributes atributy, QXmlStreamAttributes atributyTr, QXmlStreamAttributes atributyTraj, int &counter)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="bod";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("zvd",atributyTr.value("zvd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("u1",atributyTr.value("u1").toString(),"Integer"));
    if(atributyTr.value("u1").isEmpty())
    {
        qDebug()<<"Empty node";
    }
    polozky.push_back(inicializujPolozku("z1",atributyTr.value("z1").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("u2",atributyTr.value("u2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z2",atributyTr.value("z2").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("var",atributyTr.value("var").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributyTr.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("c",atributyTraj.value("c").toString(),"Integer"));


    polozky.push_back(inicializujPolozku("lon",atributy.value("lon").toString(),"String"));
    polozky.push_back(inicializujPolozku("lat",atributy.value("lat").toString(),"String"));
    polozky.push_back(inicializujPolozku("poradi",QString::number(counter),"Integer"));


    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query;
    query.exec(queryString);
    //qDebug().noquote()<<queryString;
    counter++;

    return 1;
}