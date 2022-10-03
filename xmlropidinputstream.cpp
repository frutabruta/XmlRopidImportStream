#include <QtSql>
#include "XmlRopidImportStream.h"

#include <QtXml>
#include <QDebug>
#include <QMainWindow>
#include <QCoreApplication>


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
    Navrat ahoj;
    ahoj.typ=typ;
    ahoj.obsah=obsah;
    ahoj.nazevSloupce=nazevSloupce;

    if (ahoj.typ=="String")
    {
        ahoj.obsah=overString(obsah);
    }
    if (ahoj.typ=="Integer")
    {
        ahoj.obsah=overInteger(obsah);
    }
    if (ahoj.typ=="Boolean")
    {
        ahoj.obsah=overBoolean(obsah);
    }

    return ahoj;
}


void XmlRopidImportStream::otevriSoubor(QString cesta)
{
    qDebug() <<  Q_FUNC_INFO;

    qDebug()<<"p1";
    QFile file(cesta);
    qDebug()<<"p2";


    if (!file.open(QIODevice::ReadOnly))
    {
        emit odesliChybovouHlasku("soubor se nepovedlo otevrit");
        qDebug()<<"fail1";
        return;
    }
    qDebug()<<"p3";

    ///

    // natahniOld(file);
    natahniNew(file);

    ///

    ropidSQL.zavriDB();
}


//nacesty
//SELECT s.s, s.l, s.p, s.c,  vyber.pocet, vyber.nacestne  FROM s LEFT JOIN ( SELECT xalias.s_id, SUM(xalias.na) AS nacestne, COUNT(*) AS pocet FROM x AS xalias GROUP BY xalias.s_id) AS vyber ON s.s=vyber.s_id WHERE pocet>5 AND nacestne=0;


void XmlRopidImportStream::natahniNew(QFile &file)
{
    qDebug() <<  Q_FUNC_INFO;

    QXmlStreamReader reader(&file);
    QTextStream errorStream(stderr);
    QVector<int> navazneSpojeObehu;
    // emit odesliChybovouHlasku("Zacatek importu:"+QTime::currentTime().toString() );

    this->databazeStart();
    QString staryTag="";

    if(ropidSQL.mojeDatabaze.transaction())
    {
        reader.readElementText(QXmlStreamReader::IncludeChildElements);

        QString aktualniCisloSpoje="";
        QXmlStreamAttributes atributyObehu;
        int counter=0;

        while (!reader.atEnd()) {



            auto currentToken = reader.tokenString();



            QXmlStreamAttributes atributy=reader.attributes();

            /*
            //    qDebug() << currentToken<<" "<<reader.name()<<" ";

             foreach(QXmlStreamAttribute atribut,atributy)
            {
                qDebug()<<"atribut "<<atribut.name()<<" "<<atribut.value();

            }
*/
            //qDebug()<<"hodnota v N:"<<atributy.value("n");


            if(currentToken=="StartElement")
            {
                if(staryTag=="")
                {
                    staryTag=reader.name().toString();
                }

                QVector<QString> obsah={"", "ds","x","v","s","bod","traj"};

                if((staryTag!=reader.name().toString())&&(!obsah.contains(reader.name().toString())))
                {
                    QString hlaska="Zpracovavam tag: "+reader.name().toString();
                    qDebug()<<hlaska;
                    staryTag=reader.name().toString();
                    emit odesliChybovouHlasku(hlaska);
                  //  QEventLoop::processEvents(QEventLoop::AllEvents);
                }



                if(reader.name()=="d")
                {
                    vlozDNew(atributy);
                }
                else if(reader.name()=="dd")
                {
                    vlozDdNew(atributy);
                }
                else if(reader.name()=="ids")
                {
                    vlozIdsNew(atributy);
                }
                else if(reader.name()=="m")
                {
                    vlozMNew(atributy);
                }
                else if(reader.name()=="z")
                {
                    vlozZNew(atributy);
                }
                else if(reader.name()=="p")
                {
                    vlozPNew(atributy);
                }
                else if(reader.name()=="l")
                {
                    vlozLNew(atributy);
                }
                else if(reader.name()=="k")
                {
                    vlozKNew(atributy);
                }
                else if(reader.name()=="tv")
                {
                    vlozTvNew(atributy);
                }
                else if(reader.name()=="ty")
                {
                    vlozTyNew(atributy);
                }
                else if(reader.name()=="ch")
                {
                    vlozChNew(atributy);
                }
                else if(reader.name()=="r")
                {
                    vlozRNew(atributy);
                }
                else if(reader.name()=="po")
                {
                    vlozPoNew(atributy);
                }

                else if(reader.name()=="o")
                {
                    atributyObehu=atributy;
                    vlozONew(atributy);
                }

                else if(reader.name()=="ds")
                {
                    //vlozDsNew(atributyObehu,atributy);
                    navazneSpojeObehu.append(seznamDlouhychSpojuNew(atributy));
                }

                else if(reader.name()=="s")
                {
                    aktualniCisloSpoje=vlozSNew(atributy);
                }

                else if(reader.name()=="x")
                {
                    vlozXNew(atributy,counter,aktualniCisloSpoje);
                }


                else if(reader.name()=="t")
                {
                    vlozTNew(atributy);
                }

                else if(reader.name()=="JR_XML_EXP")
                {
                    vlozPlatnostNew(atributy, platnostOd, platnostDo);
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
                    aktualniCisloSpoje="";
                    counter=0;
                }
                if(reader.name()=="o")
                {
                    vlozSpPoNew(atributyObehu,navazneSpojeObehu);
                }
            }

            reader.readNext();
        }

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

    //emit odesliChybovouHlasku("Konec importu:"+QTime::currentTime().toString() );
}

void XmlRopidImportStream::natahniOld(QFile &file)
{
    qDebug() <<  Q_FUNC_INFO;

    QDomDocument doc("mydocument");

    if (!doc.setContent(&file))
    {
        emit odesliChybovouHlasku("soubor se nepovedlo otevrit2");
        qDebug()<<"fail2";
        file.close();
        return;
    }
    qDebug()<<"p4";
    file.close();
    qDebug()<<"p5";
    qDebug()<<"uspech";
    QDomElement koren=doc.firstChildElement();


    /*
    ropidSQL.mojeDatabaze.close();
    ropidSQL.mojeDatabaze.open();
    */
    // ropidSQL.mojeDatabaze.close();
    this->databazeStart();
    qDebug()<<"is driver available "<<QString::number(ropidSQL.mojeDatabaze.isDriverAvailable("QSQLITE"));
    qDebug()<<"je databaze otevrena "<<QString::number(ropidSQL.mojeDatabaze.isOpen());
    qDebug()<<"je databaze validni "<<QString::number(ropidSQL.mojeDatabaze.isValid());
    if(ropidSQL.mojeDatabaze.transaction())
    {
        // QSqlQuery query(ropidSQL.mojeDatabaze);
        vlozPlatnost(koren, platnostOd, platnostDo);

        vlozD(koren);
        vlozDd(koren);
        vlozIds(koren);
        vlozM(koren);
        vlozZ(koren);
        vlozP(koren);
        vlozL(koren);
        vlozK(koren);
        vlozTv(koren);
        vlozTy(koren);
        vlozCh(koren);
        vlozR(koren);
        vlozPo(koren);
        vlozO(koren);
        vlozS(koren);
        vlozT(koren);

        //emit odesliChybovouHlasku("Import hotov!");

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



int XmlRopidImportStream::truncateTable(QString tabulka)
{
    qDebug() <<  Q_FUNC_INFO;
    QString queryString = ("DELETE FROM ");
    queryString+=tabulka;
    qDebug().noquote()<<queryString;
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;
}

int XmlRopidImportStream::truncateAll()
{
    qDebug() <<  Q_FUNC_INFO;
    ropidSQL.pripoj();

    //  truncateTable("``");

    truncateTable("`t`");
    truncateTable("`sp_po`");
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
    truncateTable("`hlavicka`");


    emit odesliChybovouHlasku("databaze vymazana");
    //truncateTable("");
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


int XmlRopidImportStream::vlozD(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("d");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku D je "<<pocetPrvku;

    for (int i=0;i<pocetPrvku;i++)
    {
        qDebug()<<m.at(i).toElement().attribute("n");
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("kj",element.attribute("kj"),"String"));
        polozky.push_back(inicializujPolozku("ncis",element.attribute("ncis"),"String"));
        polozky.push_back(inicializujPolozku("ico",element.attribute("ico"),"Integer"));
        polozky.push_back(inicializujPolozku("dic",element.attribute("dic"),"String"));
        polozky.push_back(inicializujPolozku("ul",element.attribute("ul"),"String"));
        polozky.push_back(inicializujPolozku("me",element.attribute("me"),"String"));
        polozky.push_back(inicializujPolozku("psc",element.attribute("psc"),"String"));
        polozky.push_back(inicializujPolozku("tel",element.attribute("tel"),"String"));
        polozky.push_back(inicializujPolozku("em",element.attribute("em"),"String"));
        QString queryString=this->slozInsert("d",polozky);
        qDebug()<<"d2 "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    qDebug()<<"konecImportuD";
    emit odesliChybovouHlasku("dokoncen import D");
    return 1;
}

int XmlRopidImportStream::vlozDNew(QXmlStreamAttributes atributy)
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

    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);


    return 1;

}


int XmlRopidImportStream::vlozDd(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("dd");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku Dd je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {

        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("z",element.attribute("z"),"String"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        QString queryString=this->slozInsert("dd",polozky);
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    qDebug()<<"konecImportuDd";
    return 1;
}

int XmlRopidImportStream::vlozDdNew(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    QString queryString=this->slozInsert("dd",polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;
}


int XmlRopidImportStream::vlozCh(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ch";
    QDomNodeList m=koren.elementsByTagName(nazevElementu);
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku "+nazevElementu+" je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));


        QString queryString=this->slozInsert(nazevElementu,polozky);
        qDebug()<<nazevElementu<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import "+nazevElementu);
    return 1;
}

int XmlRopidImportStream::vlozChNew(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ch";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}



int XmlRopidImportStream::vlozIds(QDomElement koren)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ids";
    QDomNodeList m=koren.elementsByTagName(nazevElementu);
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku "+nazevElementu+" je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("z",element.attribute("z"),"String"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("tapoj",element.attribute("tapoj"),"String"));


        QString queryString=this->slozInsert(nazevElementu,polozky);
        qDebug()<<nazevElementu<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import "+nazevElementu);
    return 1;
}

int XmlRopidImportStream::vlozIdsNew(QXmlStreamAttributes atributy)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ids";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("tapoj",atributy.value("tapoj").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}



int XmlRopidImportStream::vlozK(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="k";
    QDomNodeList m=koren.elementsByTagName(nazevElementu);
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku "+nazevElementu+" je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));

        QString queryString=this->slozInsert(nazevElementu,polozky);
        qDebug()<<nazevElementu<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import "+nazevElementu);
    return 1;
}

int XmlRopidImportStream::vlozKNew(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="k";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}



int XmlRopidImportStream::vlozM(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="m";
    QDomNodeList m=koren.elementsByTagName(nazevElementu);
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku "+nazevElementu+" je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));

        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("z",element.attribute("z"),"String"));



        QString queryString=this->slozInsert(nazevElementu,polozky);
        qDebug()<<nazevElementu<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import "+nazevElementu);
    return 1;
}


int XmlRopidImportStream::vlozMNew(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="m";

    QVector<Navrat> polozky;

    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"String"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}

int XmlRopidImportStream::vlozTv(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("tv");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku Tv2 je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("z",element.attribute("z"),"String"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("dd",element.attribute("dd"),"Integer"));
        polozky.push_back(inicializujPolozku("np",element.attribute("np"),"Boolean"));
        QString queryString=this->slozInsert("tv",polozky);
        qDebug()<<"TV2 "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import Tv2");
    return 1;
}


int XmlRopidImportStream::vlozTvNew(QXmlStreamAttributes atributy)
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
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}

int XmlRopidImportStream::vlozO(QDomElement koren)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("o");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku O je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("l",element.attribute("l"),"Integer"));
        polozky.push_back(inicializujPolozku("p",element.attribute("p"),"Integer"));
        polozky.push_back(inicializujPolozku("kj",element.attribute("kj"),"String"));
        //přepsat pro vložení spojů jednotlivě!
        polozky.push_back(inicializujPolozku("sp",element.attribute("sp"),"String"));
        polozky.push_back(inicializujPolozku("tv",element.attribute("tv"),"Integer"));
        polozky.push_back(inicializujPolozku("td",element.attribute("td"),"Integer"));
        vlozSpPo(element);
        QString queryString=this->slozInsert("o",polozky);
        qDebug()<<"O "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import O");
    return 1;
}
/* potreba slozitejsi implementace
*/
int XmlRopidImportStream::vlozONew(QXmlStreamAttributes atributy)
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
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}


int XmlRopidImportStream::vlozPlatnost(QDomElement koren, QDate &plOd, QDate &plDo)
{
    //   qDebug() <<  Q_FUNC_INFO;


    plOd=QDate::fromString(koren.attribute("od"),Qt::ISODate);
    plDo=QDate::fromString(koren.attribute("do"),Qt::ISODate);


    // QString queryString("INSERT INTO hlavicka(platnostod,platnostdo) VALUES("+ plOd.toString(Qt::ISODate)+","+plDo.toString(Qt::ISODate)+")");

    QDomElement element = koren;
    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("od",element.attribute("od"),"String"));
    polozky.push_back(inicializujPolozku("do",element.attribute("do"),"String"));
    polozky.push_back(inicializujPolozku("režim",element.attribute("režim"),"String"));
    QString queryString=this->slozInsert("hlavicka",polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    qDebug().noquote()<<queryString;


    QString vystup="platnost dat od "+plOd.toString(Qt::ISODate)+" do "+plDo.toString(Qt::ISODate);
    //   QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    qDebug()<<vystup;
    emit odesliChybovouHlasku(vystup );

    return 1;
}

int XmlRopidImportStream::vlozPlatnostNew(QXmlStreamAttributes atributy, QDate &plOd, QDate &plDo)
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
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    qDebug().noquote()<<queryString;


    QString vystup="platnost dat od "+plOd.toString(Qt::ISODate)+" do "+plDo.toString(Qt::ISODate);
    //   QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    qDebug()<<vystup;
    emit odesliChybovouHlasku(vystup );

    return 1;
}


int XmlRopidImportStream::vlozPo(QDomElement koren)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="po";
    QDomNodeList m=koren.elementsByTagName(nazevElementu);
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku "+nazevElementu+" je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("t",element.attribute("t"),"String"));
        polozky.push_back(inicializujPolozku("zkr1",element.attribute("zkr1"),"String"));
        polozky.push_back(inicializujPolozku("zkr2",element.attribute("zkr2"),"String"));
        polozky.push_back(inicializujPolozku("ois",element.attribute("ois"),"Boolean"));
        polozky.push_back(inicializujPolozku("zjr",element.attribute("zjr"),"Boolean"));
        polozky.push_back(inicializujPolozku("csad",element.attribute("csad"),"Boolean"));
        polozky.push_back(inicializujPolozku("ljr",element.attribute("ljr"),"Boolean"));
        polozky.push_back(inicializujPolozku("kjr",element.attribute("kjr"),"Boolean"));
        polozky.push_back(inicializujPolozku("jdf",element.attribute("jdf"),"Boolean"));
        polozky.push_back(inicializujPolozku("tt",element.attribute("tt"),"Boolean"));

        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("tn",element.attribute("tn"),"String"));
        polozky.push_back(inicializujPolozku("nl",element.attribute("nl"),"String"));
        polozky.push_back(inicializujPolozku("anl",element.attribute("anl"),"Integer"));
        polozky.push_back(inicializujPolozku("dd",element.attribute("dd"),"Integer"));
        polozky.push_back(inicializujPolozku("cd",element.attribute("cd"),"String"));

        QString queryString=this->slozInsert(nazevElementu,polozky);
        qDebug()<<nazevElementu<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import "+nazevElementu);
    return 1;
}

int XmlRopidImportStream::vlozPoNew(QXmlStreamAttributes atributy)
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

    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("tn",atributy.value("tn").toString(),"String"));
    polozky.push_back(inicializujPolozku("nl",atributy.value("nl").toString(),"String"));
    polozky.push_back(inicializujPolozku("anl",atributy.value("anl").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("dd",atributy.value("dd").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("cd",atributy.value("cd").toString(),"String"));
    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}

int XmlRopidImportStream::vlozR(QDomElement koren)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="r";
    QDomNodeList m=koren.elementsByTagName(nazevElementu);
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku "+nazevElementu+" je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("pop",element.attribute("pop"),"String"));

        QString queryString=this->slozInsert(nazevElementu,polozky);
        qDebug()<<nazevElementu<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import "+nazevElementu);
    return 1;
}

int XmlRopidImportStream::vlozRNew(QXmlStreamAttributes atributy)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="r";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("pop",atributy.value("pop").toString(),"String"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}


int XmlRopidImportStream::vlozTy(QDomElement koren)
{
    //   qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ty";
    QDomNodeList m=koren.elementsByTagName(nazevElementu);
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku "+nazevElementu+" je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("li",element.attribute("li"),"String"));

        QString queryString=this->slozInsert(nazevElementu,polozky);
        qDebug()<<nazevElementu<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import "+nazevElementu);
    return 1;
}

int XmlRopidImportStream::vlozTyNew(QXmlStreamAttributes atributy)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="ty";

    QVector<Navrat> polozky;
    polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("li",atributy.value("li").toString(),"String"));

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}



int XmlRopidImportStream::vlozSpPo(QDomElement koren)
{
    // qDebug() <<  Q_FUNC_INFO;
    QDomElement element = koren;


    QString spoje="";
    //2971 2972 2973 2974 2975 2976 2977 2978 2979 2980 2981 2982 2983 2984 2985 2986 2987 2988 2989 2990 2991 2992 2993 2994 2995 2996 2997 2998 2999 3000 3001";
    spoje=element.attribute("sp");
    QStringList seznam=spoje.split(" ");

    QString linka=element.attribute("l");
    QString poradi=element.attribute("p");
    QString kalendar=element.attribute("kj");

    int pocetPrvku=seznam.count();
    qDebug()<<"pocet spoju v obehu "+element.attribute("l")+"/"+element.attribute("p")+" je "<<pocetPrvku;

    QDomNodeList dlouheSpoje=element.elementsByTagName("ds");

    QVector<int> navazujiciSpoje=seznamDlouhychSpoju(dlouheSpoje);

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
        //vlozSpPo(element);
        QString queryString=this->slozInsert("sp_po",polozky);
        qDebug()<<"sp_po "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }

    qDebug()<<"konecImportu sp_po";
    return 1;
}



int XmlRopidImportStream::vlozSpPoNew(QXmlStreamAttributes atributyO,QVector<int> navazujiciSpoje)
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
        //vlozSpPo(element);
        QString queryString=this->slozInsert("sp_po",polozky);
       // qDebug()<<"sp_po "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }

    return 1;
}


int XmlRopidImportStream::vlozDsNew(QXmlStreamAttributes atributyO,QXmlStreamAttributes atributyDs)
{
    //  qDebug() <<  Q_FUNC_INFO;

    QString spoje="";
    //2971 2972 2973 2974 2975 2976 2977 2978 2979 2980 2981 2982 2983 2984 2985 2986 2987 2988 2989 2990 2991 2992 2993 2994 2995 2996 2997 2998 2999 3000 3001";
    spoje=atributyO.value("sp").toString();
    QStringList seznam=spoje.split(" ");

    QString linka=atributyO.value("l").toString();
    QString poradi=atributyO.value("p").toString();
    QString kalendar=atributyO.value("kj").toString();

    int pocetPrvku=seznam.count();
    //    qDebug()<<"pocet spoju v obehu "+atributyO.value("l")+"/"+atributyO.value("p")+" je "<<pocetPrvku;



    QVector<int> navazujiciSpoje=seznamDlouhychSpojuNew(atributyDs);

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
        //vlozSpPo(element);
        QString queryString=this->slozInsert("sp_po",polozky);
        qDebug()<<"sp_po "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }

    return 1;
}

QVector<int> XmlRopidImportStream::seznamDlouhychSpoju(QDomNodeList &dlouheSpoje)
{
    //  qDebug() <<  Q_FUNC_INFO;
    QVector<int> navazujiciSpoje;

    for(int i=0;i<dlouheSpoje.length();i++)
    {
        QString retezec= dlouheSpoje.at(i).toElement().attribute("sp");
        qDebug()<<"retezec dlouhych spoju "<<retezec;
        QStringList seznam= retezec.split(" ");

        if (seznam.length()>1)
        {
            seznam.removeLast();

            for (int j=0;j<seznam.length();j++)
            {
                navazujiciSpoje.append(seznam.at(j).toInt());
                qDebug()<<"navazujici spoj "<<seznam.at(j).toInt();
            }
        }
        else
        {
            qDebug()<<"seznam dlouhych spoju je prazdny";
        }

    }

    return navazujiciSpoje;

}

QVector<int> XmlRopidImportStream::seznamDlouhychSpojuNew(QXmlStreamAttributes atributy)
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



int XmlRopidImportStream::vlozL(QDomElement koren)
{
    //qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("l");
    int pocetPrvku=m.count();

    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("d",element.attribute("d"),"Integer"));
        polozky.push_back(inicializujPolozku("kj",element.attribute("kj"),"String"));
        polozky.push_back(inicializujPolozku("lc",element.attribute("lc"),"Integer"));
        polozky.push_back(inicializujPolozku("tl",element.attribute("tl"),"String"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("kup",element.attribute("kup"),"Integer"));
        polozky.push_back(inicializujPolozku("ids",element.attribute("ids"),"Boolean"));
        polozky.push_back(inicializujPolozku("noc",element.attribute("noc"),"Boolean"));
        polozky.push_back(inicializujPolozku("aois",element.attribute("aois"),"String"));
        polozky.push_back(inicializujPolozku("a",element.attribute("a"),"String"));
        polozky.push_back(inicializujPolozku("kli",element.attribute("kli"),"Integer"));
        polozky.push_back(inicializujPolozku("cids",element.attribute("cids"),"String"));
        QString queryString=this->slozInsert("l",polozky);
        qDebug()<<"l2 "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import L");
    qDebug()<<"konecImportuL";
    return 1;
}

int XmlRopidImportStream::vlozLNew(QXmlStreamAttributes atributy)
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
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}



int XmlRopidImportStream::vlozP(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("p");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku P je "<<pocetPrvku;

    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("kj",element.attribute("kj"),"String"));
        polozky.push_back(inicializujPolozku("z",element.attribute("z"),"String"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("d",element.attribute("d"),"Integer"));
        polozky.push_back(inicializujPolozku("dd",element.attribute("dd"),"Integer"));
        polozky.push_back(inicializujPolozku("u",element.attribute("u"),"Integer"));
        polozky.push_back(inicializujPolozku("mail",element.attribute("mail"),"String"));
        polozky.push_back(inicializujPolozku("telv",element.attribute("telg"),"String"));
        polozky.push_back(inicializujPolozku("telg",element.attribute("telv"),"String"));
        polozky.push_back(inicializujPolozku("telz",element.attribute("telz"),"String"));
        QString queryString=this->slozInsert("p",polozky);
        qDebug()<<"p2 "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import P");
    qDebug()<<"konecImportuP";
    return 1;
}


int XmlRopidImportStream::vlozPNew(QXmlStreamAttributes atributy)
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
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;
}


int XmlRopidImportStream::vlozZ(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("z");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku Z je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        qDebug()<<m.at(i).toElement().attribute("n");
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("u",element.attribute("u"),"Integer"));
        polozky.push_back(inicializujPolozku("z",element.attribute("z"),"Integer"));
        polozky.push_back(inicializujPolozku("kj",element.attribute("kj"),"String"));
        polozky.push_back(inicializujPolozku("n",element.attribute("n"),"String"));
        polozky.push_back(inicializujPolozku("pop",element.attribute("pop"),"String"));
        polozky.push_back(inicializujPolozku("cis",element.attribute("cis"),"Integer"));
        polozky.push_back(inicializujPolozku("ois",element.attribute("ois"),"Integer"));
        polozky.push_back(inicializujPolozku("co",element.attribute("co"),"Integer"));
        polozky.push_back(inicializujPolozku("no",element.attribute("no"),"String"));
        polozky.push_back(inicializujPolozku("spz",element.attribute("spz"),"String"));
        polozky.push_back(inicializujPolozku("ids",element.attribute("ids"),"Integer"));
        polozky.push_back(inicializujPolozku("ids2",element.attribute("ids2"),"Integer"));
        polozky.push_back(inicializujPolozku("ids3",element.attribute("ids3"),"Integer"));
        polozky.push_back(inicializujPolozku("tp",element.attribute("tp"),"String"));
        polozky.push_back(inicializujPolozku("tp2",element.attribute("tp2"),"String"));
        polozky.push_back(inicializujPolozku("tp3",element.attribute("tp3"),"String"));
        polozky.push_back(inicializujPolozku("sx",element.attribute("sx"),"String"));
        polozky.push_back(inicializujPolozku("sy",element.attribute("sx"),"String"));
        polozky.push_back(inicializujPolozku("lat",element.attribute("lat"),"String"));
        polozky.push_back(inicializujPolozku("lng",element.attribute("lng"),"String"));
        polozky.push_back(inicializujPolozku("sta",element.attribute("sta"),"String"));
        polozky.push_back(inicializujPolozku("m",element.attribute("m"),"Integer"));
        polozky.push_back(inicializujPolozku("bbn",element.attribute("bbn"),"Boolean"));
        polozky.push_back(inicializujPolozku("kidos",element.attribute("kidos"),"Integer"));
        polozky.push_back(inicializujPolozku("st",element.attribute("st"),"String"));
        polozky.push_back(inicializujPolozku("xA",element.attribute("xA"),"Boolean"));
        polozky.push_back(inicializujPolozku("xB",element.attribute("xB"),"Boolean"));
        polozky.push_back(inicializujPolozku("xC",element.attribute("xC"),"Boolean"));
        polozky.push_back(inicializujPolozku("xVla",element.attribute("xVla"),"Boolean"));
        polozky.push_back(inicializujPolozku("xLod",element.attribute("xLod"),"Boolean"));
        polozky.push_back(inicializujPolozku("xLet",element.attribute("xLed"),"Boolean"));

        QString queryString=this->slozInsert("z",polozky);
        qDebug()<<"z2 "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import Z");
    qDebug()<<"konecImportuZ";
    return 1;
}


int XmlRopidImportStream::vlozZNew(QXmlStreamAttributes atributy)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="z";

    QVector<Navrat> polozky;
    //  polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));

    polozky.push_back(inicializujPolozku("u",atributy.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("kj",atributy.value("kj").toString(),"String"));
    polozky.push_back(inicializujPolozku("n",atributy.value("n").toString(),"String"));
    polozky.push_back(inicializujPolozku("pop",atributy.value("pop").toString(),"String"));
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

    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}

int XmlRopidImportStream::vlozS(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("s");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku S je "<<pocetPrvku;

    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("s",element.attribute("s"),"Integer"));
        polozky.push_back(inicializujPolozku("id",element.attribute("id"),"Integer"));
        polozky.push_back(inicializujPolozku("l",element.attribute("l"),"Integer"));
        polozky.push_back(inicializujPolozku("p",element.attribute("p"),"Integer"));
        polozky.push_back(inicializujPolozku("dd",element.attribute("dd"),"Integer"));
        polozky.push_back(inicializujPolozku("pr",element.attribute("pr"),"Integer"));
        polozky.push_back(inicializujPolozku("d",element.attribute("d"),"Integer"));
        polozky.push_back(inicializujPolozku("tv",element.attribute("tv"),"Integer"));
        polozky.push_back(inicializujPolozku("kj",element.attribute("kj"),"String"));
        polozky.push_back(inicializujPolozku("ty",element.attribute("ty"),"Integer"));
        polozky.push_back(inicializujPolozku("ch",element.attribute("ch"),"Integer"));
        polozky.push_back(inicializujPolozku("ids",element.attribute("ids"),"Boolean"));
        polozky.push_back(inicializujPolozku("vy",element.attribute("vy"),"Boolean"));
        polozky.push_back(inicializujPolozku("man",element.attribute("man"),"Boolean"));
        polozky.push_back(inicializujPolozku("c",element.attribute("c"),"Integer"));
        polozky.push_back(inicializujPolozku("neve",element.attribute("neve"),"Boolean"));
        polozky.push_back(inicializujPolozku("ns",element.attribute("ns"),"Integer"));
        QString queryString=this->slozInsert("s",polozky);
        //qDebug()<<"s2 "<<queryString;

        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
        vlozX(m.at(i).toElement());
    }
    emit odesliChybovouHlasku("dokoncen import S");

    qDebug()<<"konecImportuS";
    return 1;
}


QString XmlRopidImportStream::vlozSNew(QXmlStreamAttributes atributy)
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
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);




    return atributy.value("s").toString();
}


int XmlRopidImportStream::vlozT(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("t");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku T je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {

        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;

        polozky.push_back(inicializujPolozku("u",element.attribute("u"),"Integer"));
        polozky.push_back(inicializujPolozku("z",element.attribute("z"),"Integer"));
        polozky.push_back(inicializujPolozku("kj",element.attribute("kj"),"String"));
        polozky.push_back(inicializujPolozku("ois",element.attribute("ois"),"Integer"));
        polozky.push_back(inicializujPolozku("cis",element.attribute("cis"),"Integer"));
        polozky.push_back(inicializujPolozku("nza",element.attribute("nza"),"String"));
        polozky.push_back(inicializujPolozku("ri",element.attribute("ri"),"String"));
        polozky.push_back(inicializujPolozku("ji",element.attribute("ji"),"String"));
        polozky.push_back(inicializujPolozku("vtm",element.attribute("vtm"),"String"));
        polozky.push_back(inicializujPolozku("vtn",element.attribute("vtn"),"String"));
        polozky.push_back(inicializujPolozku("btm",element.attribute("btm"),"String"));
        polozky.push_back(inicializujPolozku("btn",element.attribute("btn"),"String"));
        polozky.push_back(inicializujPolozku("ctm",element.attribute("ctm"),"String"));
        polozky.push_back(inicializujPolozku("ctn",element.attribute("ctn"),"String"));
        polozky.push_back(inicializujPolozku("lcdm",element.attribute("lcdm"),"String"));
        polozky.push_back(inicializujPolozku("lcdn",element.attribute("lcdn"),"String"));
        polozky.push_back(inicializujPolozku("hl",element.attribute("hl"),"String"));

        QString queryString=this->slozInsert("t",polozky);
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    emit odesliChybovouHlasku("dokoncen import T");
    qDebug()<<"konecImportuT";

    return 1;
}

int XmlRopidImportStream::vlozTNew(QXmlStreamAttributes atributy)
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
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

    return 1;

}

int XmlRopidImportStream::vlozX(QDomElement koren)
{
    qDebug() <<  Q_FUNC_INFO;
    QDomNodeList m=koren.elementsByTagName("x");
    int pocetPrvku=m.count();
    qDebug()<<"pocet prvku X je "<<pocetPrvku;
    for (int i=0;i<pocetPrvku;i++)
    {
        QDomElement prvek=m.at(i).toElement();
        QDomElement element = m.at(i).toElement();
        QVector<Navrat> polozky;
        polozky.push_back(inicializujPolozku("s_id",koren.attribute("s"),"Integer"));
        polozky.push_back(inicializujPolozku("u",element.attribute("u"),"Integer"));
        polozky.push_back(inicializujPolozku("z",element.attribute("z"),"Integer"));
        polozky.push_back(inicializujPolozku("p",element.attribute("p"),"Integer"));
        polozky.push_back(inicializujPolozku("o",element.attribute("o"),"Integer"));
        polozky.push_back(inicializujPolozku("t",element.attribute("t"),"String"));
        polozky.push_back(inicializujPolozku("ty",element.attribute("ty"),"Integer"));
        polozky.push_back(inicializujPolozku("ces",element.attribute("ces"),"Boolean"));
        polozky.push_back(inicializujPolozku("zn",element.attribute("zn"),"Boolean"));
        polozky.push_back(inicializujPolozku("na",element.attribute("na"),"Boolean"));
        polozky.push_back(inicializujPolozku("vyst",element.attribute("vyst"),"Boolean"));
        polozky.push_back(inicializujPolozku("nast",element.attribute("nast"),"Boolean"));
        polozky.push_back(inicializujPolozku("xA",element.attribute("xA"),"Boolean"));
        polozky.push_back(inicializujPolozku("xB",element.attribute("xB"),"Boolean"));
        polozky.push_back(inicializujPolozku("xC",element.attribute("xC"),"Boolean"));
        polozky.push_back(inicializujPolozku("xD",element.attribute("xC"),"Boolean"));
        polozky.push_back(inicializujPolozku("xVla",element.attribute("xVla"),"Boolean"));
        polozky.push_back(inicializujPolozku("xLod",element.attribute("xLod"),"Boolean"));
        polozky.push_back(inicializujPolozku("xLet",element.attribute("xLed"),"Boolean"));
        polozky.push_back(inicializujPolozku("xorder",QString::number(i),"Integer"));
        polozky.push_back(inicializujPolozku("zsol",element.attribute("zsol"),"Boolean"));
        polozky.push_back(inicializujPolozku("s1",element.attribute("s1"),"Boolean"));
        polozky.push_back(inicializujPolozku("s2",element.attribute("s2"),"Boolean"));

        QString queryString=this->slozInsert("x",polozky);
        //qDebug()<<"x2 "<<queryString;
        QSqlQuery query(queryString,ropidSQL.mojeDatabaze);
    }
    qDebug()<<"konecImportuX";

    return 1;
}


int XmlRopidImportStream::vlozXNew(QXmlStreamAttributes atributy, int &counter, QString cisloSpoje)
{
    //qDebug() <<  Q_FUNC_INFO;
    QString nazevElementu="x";

    QVector<Navrat> polozky;
    //   polozky.push_back(inicializujPolozku("c",atributy.value("c").toString(),"Integer"));

    polozky.push_back(inicializujPolozku("s_id",cisloSpoje,"Integer"));
    polozky.push_back(inicializujPolozku("u",atributy.value("u").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("z",atributy.value("z").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("p",atributy.value("p").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("o",atributy.value("o").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("t",atributy.value("t").toString(),"String"));
    polozky.push_back(inicializujPolozku("ty",atributy.value("ty").toString(),"Integer"));
    polozky.push_back(inicializujPolozku("ces",atributy.value("ces").toString(),"Boolean"));
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


    QString queryString=this->slozInsert(nazevElementu,polozky);
    QSqlQuery query(queryString,ropidSQL.mojeDatabaze);

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
