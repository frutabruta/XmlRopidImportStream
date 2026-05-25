#ifndef SQLITEBASE_H
#define SQLITEBASE_H
#include <QObject>
#include <QtSql>


class SqLiteBase : public QObject
{
    Q_OBJECT
public:
    SqLiteBase();
    QSqlDatabase dbFile;

    QString dbFileName="";
    QString dbFolderPath="";
    QString dbFilePath="";

    int initialize();
    int dbOpen();
    int dbClose();

    bool executeQuery(QString queryString);
    bool executeQuery(QSqlQuery &query);

    QSqlQuery prepareAndExec(const QString &queryString, const QVariantMap &bindings={});

    bool transactionStart();
    bool transactionStop();

    bool tableCreate(QString tableName, QVector<QString> columnNames);
    bool tableDelete(QString tableName, QVector<QString> columnNames);

    bool insertDataRow(QString nazevTabulky, QVector<QString> hlavicka, QVector<QString> data);

    bool didQueryFail(QSqlQuery &query);

    QString secondsStringToHhMm(QString input);
    QString secondsStringToHhMmSs(QString input);
    QString fillWithZeroes(int number, int expectedLength);
    bool isDateInRange(QDate selectedDate, QDate startDate, QDate endDate);

    int vacuum();
    int truncateTable(QString tableName);

protected:
    QString mConnectionName = "";

    QString threadConnectionName() const;
signals:
    void signalErrorMessage(QString chybovaHlaska);

};

#endif // SQLITEBASE_H
