#include "sqlitebase.h"

SqLiteBase::SqLiteBase()
{
    dbFileName="data.sqlite";
    dbFolderPath=QCoreApplication::applicationDirPath();
    dbFilePath=dbFolderPath+"/"+dbFileName;
    mConnectionName = "SqLiteBase_" + dbFilePath;
}



int SqLiteBase::initialize()

{
    qDebug()<< Q_FUNC_INFO;

    //this->mojeDatabaze.setHostName(adresa);
    //this->mojeDatabaze.setPort(3306);
    //this->mojeDatabaze.setHostName("127.0.0.1");

    qDebug()<<"db file path:"<<dbFilePath;

    bool ok = dbOpen();
    if (ok)
    {
        qDebug()<<"connection ok";
        return 1;
    }
    else
    {
        emit signalErrorMessage("connection failed");
        qDebug()<<"connection failed";
        return 0;
    }

}

/*!

*/
int SqLiteBase::dbOpen()
{
    qDebug() << Q_FUNC_INFO;

    // Create a unique connection name per thread
    QString connectionName = QString("%1_%2")
                                 .arg(mConnectionName)
                                 .arg(reinterpret_cast<quint64>(QThread::currentThread()), 0, 16);

    // If this thread already has an open connection, reuse it
    if (QSqlDatabase::contains(connectionName))
    {
        this->dbFile = QSqlDatabase::database(connectionName);
        if (this->dbFile.isOpen())
        {
            qDebug() << "Reusing existing connection:" << connectionName;
            return 1;
        }
    }
    else
    {
        this->dbFile = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        this->dbFile.setDatabaseName(dbFilePath);
    }

    if (this->dbFile.open())
    {
        qDebug() << "Opened connection:" << connectionName;
        qDebug() << "is driver available " << QString::number(dbFile.isDriverAvailable("QSQLITE"));
        qDebug() << "is db valid "         << QString::number(dbFile.isValid());
        return 1;
    }
    else
    {
        emit signalErrorMessage("DB open failed: " + dbFile.lastError().text());
        qDebug() << "DB open failed " << dbFile.lastError();
    }

    return 0;
}

bool SqLiteBase::didQueryFail(QSqlQuery &query)
{
    qDebug()<< Q_FUNC_INFO;
    QString dbError=query.lastError().databaseText();
    QString dbDriverError=query.lastError().driverText();

    if((dbError=="")&&(dbDriverError==""))
    {
        return 0;
    }

    emit signalErrorMessage("db error: "+dbError+" "+dbDriverError);
    qDebug()<<"db error: "<<dbError<<" "<<dbDriverError;
    return 1;
}


bool SqLiteBase::tableCreate(QString tableName, QVector<QString> columnNames)
{
    qDebug()<< Q_FUNC_INFO;

    if((tableName=="")||(columnNames.isEmpty()))
    {
        return false;
    }

    QString queryText=QString("CREATE TABLE %1 (%2) ;").arg(tableName,columnNames.join(","));

    qDebug()<<queryText;

    return executeQuery(queryText);
}

bool SqLiteBase::tableDelete(QString tableName, QVector<QString> columnNames)
{
    qDebug()<< Q_FUNC_INFO;

    if((tableName=="")||(columnNames.isEmpty()))
    {
        return false;
    }

    QString queryText=QString("DROP TABLE IF EXISTS %1 ;").arg(tableName);

    qDebug()<<queryText;

    return executeQuery(queryText);


}


/*!

*/
int SqLiteBase::dbClose()
{
    QString connectionName = threadConnectionName();
    if (QSqlDatabase::contains(connectionName))
    {
        QSqlDatabase::database(connectionName).close();
        QSqlDatabase::removeDatabase(connectionName);
        qDebug() << "Closed connection:" << connectionName;
    }
    return 1;
}

/*!

*/
QString SqLiteBase::secondsStringToHhMm(QString input)
{
    if(input.isEmpty())
    {
        return "";
    }

    int secondsSinceMidnight = input.toInt();
    QTime time = QTime::fromMSecsSinceStartOfDay(secondsSinceMidnight * 1000);
    return time.toString("hh:mm");
}


/*!

*/
QString SqLiteBase::secondsStringToHhMmSs(QString input)
{
    qDebug()<< Q_FUNC_INFO;

    if(input.isEmpty())
    {
        return "";
    }

    int secondsSinceMidnight = input.toInt();
    QTime time = QTime::fromMSecsSinceStartOfDay(secondsSinceMidnight * 1000);
    return time.toString("hh:mm:ss");
}


/*!

*/
QString SqLiteBase::fillWithZeroes(int number,int expectedLength)
{
    return QString::number(number).rightJustified(expectedLength, QLatin1Char('0'));
}

bool SqLiteBase::executeQuery(QString queryString)
{
    qDebug()<< Q_FUNC_INFO;
    QSqlQuery query;
    query.exec(queryString);

    if(didQueryFail(query))
    {
        return 0;
    }

    return 1;
}

bool SqLiteBase::executeQuery(QSqlQuery &query)
{
    qDebug()<< Q_FUNC_INFO;
    query.exec();
    qDebug()<<query.lastQuery();

    if(didQueryFail(query))
    {
        return 0;
    }
    return 1;
}

/*!

*/
bool SqLiteBase::isDateInRange(QDate selectedDate, QDate startDate, QDate endDate)
{

    qDebug()<< Q_FUNC_INFO;
    int rozdilZacatek=selectedDate.daysTo(startDate);
    int rozdilKonec=selectedDate.daysTo(endDate);
    qDebug()<<"pocet dni ZACATEK "<<rozdilZacatek<<" pocet dni KONEC "<<rozdilKonec;

    if ((rozdilZacatek<=0)&&(rozdilKonec>=0))
    {
        return true;
    }
    return false;
}


bool SqLiteBase::insertDataRow(QString nazevTabulky, QVector<QString> hlavicka, QVector<QString> data)
{
    QString queryText="";

    queryText=QString("INSERT INTO %1 (%2) VALUES (%3);").arg(nazevTabulky,hlavicka.join(","),data.join(","));

    return executeQuery(queryText);
}

QSqlQuery SqLiteBase::prepareAndExec(const QString &queryString,
                                     const QVariantMap &bindings)
{
    QString connectionName = threadConnectionName();

    if (!QSqlDatabase::contains(connectionName) ||
        !QSqlDatabase::database(connectionName).isOpen())
    {
        if (!dbOpen())
        {
            qWarning() << Q_FUNC_INFO << "Failed to open database for thread:" << connectionName;
            return QSqlQuery();
        }
    }

    QSqlDatabase db = QSqlDatabase::database(connectionName);
    QSqlQuery query(db);

    if (!query.prepare(queryString))
    {
        qWarning() << Q_FUNC_INFO << "Prepare failed:" << query.lastError();
        return query;
    }

    // Log the actual bound values Qt sees after binding

    if(!bindings.isEmpty())
    {    // Log all bindings
        qDebug() << Q_FUNC_INFO << "Bindings:";
        for (auto it = bindings.begin(); it != bindings.end(); ++it)
        {
            query.bindValue(it.key(), it.value());
            qDebug() << " " << it.key() << "=" << it.value();
        }
        qDebug() << Q_FUNC_INFO << "Bound values:" << query.boundValues();
    }

    if (!query.exec())
    {
        qWarning() << Q_FUNC_INFO << "Query failed:" << query.lastError();
        return query;
    }

    return query;
}

QString SqLiteBase::threadConnectionName() const
{
    return QString("%1_%2")
    .arg(mConnectionName)
        .arg(reinterpret_cast<quint64>(QThread::currentThread()), 0, 16);
}

bool SqLiteBase::transactionStop()
{
    qDebug() <<  Q_FUNC_INFO;

    QTextStream errorStream(stderr);

    if(dbFile.transaction())
    {
        return 1;
    }
    else
    {
        qDebug() << "Failed to start transaction mode";
        qDebug()<<dbFile.lastError();
    }
    return 0;
}


bool SqLiteBase::transactionStart()
{
    if(!dbFile.commit())
    {
        qDebug() << "Failed to commit";
        dbFile.rollback();
        return 0;
    }
    return 1;
}


int SqLiteBase::vacuum()
{
    qDebug() <<  Q_FUNC_INFO;
    QString queryString = ("VACUUM;");
    prepareAndExec(queryString);
    return 1;
}


int SqLiteBase::truncateTable(QString tableName)
{
    qDebug() <<  Q_FUNC_INFO;
    QString queryString = ("DELETE FROM ");
    queryString+=tableName;
    prepareAndExec(queryString);
    return 1;
}
