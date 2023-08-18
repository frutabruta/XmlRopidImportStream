#ifndef XMLIMPORTAPCDPP_H
#define XMLIMPORTAPCDPP_H

#include "xmlropidimportstream.h"

class XmlImportApcDpp: public XmlRopidImportStream
{
    void run() override {
            QString result;
            /* ... here is the expensive or blocking operation ... */
            slotOtevriSoubor(vstupniXmlSouborCesta);
            emit resultReady(result);
        }
public:
    XmlImportApcDpp();
    void natahni(QFile &file);
    int truncateAll();
private slots:
    void slotOtevriSoubor(QString cesta);
private:
    void otevriSoubor(QString cesta);
    QString nazevTabulky="apc_dpp";
};

#endif // XMLIMPORTAPCDPP_H
