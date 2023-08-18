#include "xmlimportapcopv.h"

XmlImportApcOpv::XmlImportApcOpv()
{

}


void XmlImportApcOpv::natahni(QFile &file)
{
    qDebug() <<  Q_FUNC_INFO;

    QXmlStreamReader reader(&file);
    QString fileName=file.fileName().split("/").last();
    qDebug()<<"oteviram soubor "<<fileName;


    this->databazeStart();


    if(sqLiteZaklad.mojeDatabaze.transaction())
    {
        // reader.readElementText(QXmlStreamReader::IncludeChildElements);

        QXmlStreamAttributes atributyObehu;




        bool jePtAktivni=false;
        bool jeVaktivni=false;

        QMap<QString,QString> hodnotyOpv;
        QMap<QString,QString> hodnotyPt;
        QMap<QString,QString> hodnotyV;

        while (!reader.atEnd())
        {
            QString currentToken = reader.tokenString();
            QString readerName= reader.name().toString();


            if(currentToken=="StartElement")
            {

                if(jeVaktivni)
                {
                    if(readerName=="pt")
                    {
                        jePtAktivni=true;

                    }
                    else
                    {
                        if(jePtAktivni)
                        {

                            QString klic=readerName;
                            QString hodnota=reader.readElementText(); //XXX

                            hodnotyPt[klic]=hodnota;
                            //   qDebug()<<"pridavam ["<<klic<<"]="<<hodnota;

                        }
                        else
                        {

                            //lk, por, cv
                            QString klic=readerName;
                            QString hodnota=reader.readElementText(); //XXX

                            hodnotyV[klic]=hodnota;

                        }


                    }
                }
                else
                {
                    if(readerName=="v")
                    {
                        jeVaktivni=true;
                    }

                    else if(readerName=="ts")
                    {
                        QString hodnota=reader.readElementText();
                        hodnotyOpv["ts"]=hodnota;
                    }

                    else if(readerName=="Dat")
                    {

                        QString hodnota=reader.readElementText();


                        hodnotyOpv["Dat"]=hodnota;
                    }
                    else if(readerName=="TGen")
                    {

                        QString hodnota=reader.readElementText();

                        hodnotyOpv["TGen"]=hodnota;
                        hodnotyOpv["soubor"]=fileName;
                    }


                }


            }



            else if(currentToken=="EndElement")
            {
                if(readerName=="pt")
                {
                    //vloz d
                    jePtAktivni=false;

                    QMapAppend(hodnotyPt,hodnotyV);
                    QMapAppend(hodnotyPt,hodnotyOpv);

                    this->vlozPoleAtributu("`"+nazevTabulky+"`", hodnotyPt);


                    hodnotyPt.clear();

                }
                if(readerName=="v")
                {
                    //vloz c
                    jeVaktivni=false;




                    hodnotyV.clear();
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

int XmlImportApcOpv::truncateAll()
{
    qDebug() <<  Q_FUNC_INFO;
    sqLiteZaklad.pripoj();

    //  truncateTable("``");

    truncateTable("`"+nazevTabulky+"`");

    emit odesliChybovouHlasku("databaze vymazana");
    sqLiteZaklad.zavriDB();
    return 1;


}

void XmlImportApcOpv::otevriSoubor(QString cesta)
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


void XmlImportApcOpv::slotOtevriSoubor(QString cesta)
{
    emit signalNastavProgressMax(spocitejRadkySouboru(cesta));
    otevriSoubor(cesta);
}
