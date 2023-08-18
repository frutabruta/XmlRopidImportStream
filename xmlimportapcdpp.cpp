#include "xmlimportapcdpp.h"

XmlImportApcDpp::XmlImportApcDpp()
{

}


void XmlImportApcDpp::natahni(QFile &file)
{
    qDebug() <<  Q_FUNC_INFO;

    QXmlStreamReader reader(&file);
    QString fileName=file.fileName().split("/").last();
    qDebug()<<"oteviram soubor "<<fileName;
    QTextStream errorStream(stderr);


    this->databazeStart();


    if(sqLiteZaklad.mojeDatabaze.transaction())
    {
       // reader.readElementText(QXmlStreamReader::IncludeChildElements);

        QXmlStreamAttributes atributyObehu;



        bool jeDaktivni=false;
        bool jeCaktivni=false;

        QMap<QString,QString> hodnotyDavka;
        QMap<QString,QString> hodnotyD;
        QMap<QString,QString> hodnotyC;

        while (!reader.atEnd()) {

            QString currentToken = reader.tokenString();
            QString readerName= reader.name().toString();


            if(currentToken=="StartElement")
            {
                if(jeCaktivni)
                {
                    if(readerName=="d")
                    {
                        jeDaktivni=true;

                    }
                    else
                    {
                        if(jeDaktivni)
                        {

                            QString klic=readerName;
                            QString hodnota=reader.readElementText(); //XXX

                           hodnotyD[klic]=hodnota;
                       //   qDebug()<<"pridavam ["<<klic<<"]="<<hodnota;

                        }
                        else
                        {

                            //lk, por, cv
                            QString klic=readerName;
                            QString hodnota=reader.readElementText(); //XXX

                            hodnotyC[klic]=hodnota;

                        }


                    }
                }
                else
                {
                    if(readerName=="c")
                    {
                        jeCaktivni=true;
                    }

                    else if(readerName=="Dat")
                    {

                        QString hodnota=reader.readElementText();


                        hodnotyDavka["Dat"]=hodnota;
                    }
                    else if(readerName=="TGen")
                    {

                        QString hodnota=reader.readElementText();

                        hodnotyDavka["TGen"]=hodnota;
                        hodnotyDavka["soubor"]=fileName	;
                    }


                }


            }
            else if(currentToken=="EndElement")
            {
                if(readerName=="d")
                {
                    //vloz d
                    jeDaktivni=false;

                    QMapAppend(hodnotyD,hodnotyC);
                    QMapAppend(hodnotyD,hodnotyDavka);

                    this->vlozPoleAtributu("`"+nazevTabulky+"`", hodnotyD);


                    hodnotyD.clear();

                }
                if(readerName=="c")
                {
                    //vloz c
                    jeCaktivni=false;




                    hodnotyC.clear();
                    emit signalNastavProgress(reader.lineNumber()+1);

                }


            }


            reader.readNext();
        }

        if(!sqLiteZaklad.mojeDatabaze.commit())
        {
            qDebug() << "Failed to commit";
            sqLiteZaklad.mojeDatabaze.rollback();
        }
    }
    else
    {
        qDebug() << "Failed to start transaction mode";
        qDebug()<<sqLiteZaklad.mojeDatabaze.lastError();
    }

}


int XmlImportApcDpp::truncateAll()
{
    qDebug() <<  Q_FUNC_INFO;
    sqLiteZaklad.pripoj();

    //  truncateTable("``");

    truncateTable("`"+nazevTabulky+"`");

    emit odesliChybovouHlasku("databaze vymazana");
    sqLiteZaklad.zavriDB();
    return 1;


}

void XmlImportApcDpp::otevriSoubor(QString cesta)
{
    qDebug() <<  Q_FUNC_INFO;
    QTime zacatek=QTime::currentTime();
    emit odesliChybovouHlasku("Zacatek importu:"+zacatek.toString()+ "soubor:"+cesta);


    QFile file(cesta);

    if (!file.open(QIODevice::ReadOnly))
    {
        emit odesliChybovouHlasku("soubor se nepovedlo otevrit: "+cesta);
        qDebug()<<"fail1";
        return;
    }
    else
    {
        qDebug()<<"povedlo se otevrit soubor"<<cesta;
    }

    natahni(file);

    sqLiteZaklad.zavriDB();
    QTime konec=QTime::currentTime();

    emit odesliChybovouHlasku("Konec importu:"+konec.toString()+" \n import trval vterin: "+QString::number(zacatek.secsTo(konec)) );
}


void XmlImportApcDpp::slotOtevriSoubor(QString cesta)
{
    emit signalNastavProgressMax(spocitejRadkySouboru(cesta));
    otevriSoubor(cesta);
}
