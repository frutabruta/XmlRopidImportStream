#ifndef XMLIMPORTAPCOPV_H
#define XMLIMPORTAPC_H

#include "xmlropidimportstream.h"

class XmlImportApcOpv : public XmlRopidImportStream
{
    void run() override {
            QString result;
            /* ... here is the expensive or blocking operation ... */
            slotOtevriSoubor(vstupniXmlSouborCesta);
            emit resultReady(result);
        }
public:
    XmlImportApcOpv();
    void natahni(QFile &file);
    int truncateAll();
private slots:
    void slotOtevriSoubor(QString cesta);
private:
    void otevriSoubor(QString cesta);
    QString nazevTabulky="apc_opv";
};

#endif // XMLIMPORTAPCOPV_H
