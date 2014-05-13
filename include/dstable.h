#ifndef DSTABLE_H
#define DSTABLE_H

#include <QtCore>
#include <QtDebug>
#include <QtSql>

class DsTable : public QObject{
    Q_OBJECT

public:
    static void testClass();

    typedef QHash<QString, QVariant> Record;

    enum FieldType { TEXT, INTEGER, REAL, BOOLEAN, BLOB };

    struct SchemaField {
        QString fieldName;
        DsTable::FieldType fieldType;
    };

    explicit DsTable(QObject *parent = 0);
    bool initTable(QString            tableName,
                   QList<SchemaField> fieldSchema,
                   QSqlDatabase       db);


    bool recordExists(QString lookupKey);
    void clearRecords();
    bool insertRecord(Record record);
    bool deleteRecord(QString lKey);
    int getRecordCount();
    Record getRecord(QString lookupKey);

private:
    bool loadTable();
    bool createTable();

    bool tableInitialized;
    QString tableName;
    QString lookupKey;
    QList<SchemaField> schema;
    QHash<QString, Record> records;

    QSqlDatabase db;
};

#endif // DSTABLE_H
