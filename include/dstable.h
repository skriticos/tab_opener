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

    bool contains(QString lookupKey);
    bool deleteRecord(QString lKey);
    virtual bool insertRecord(Record record);
    int size();
    QStringList getRecordKeys();
    Record getRecord(QString lookupKey);
    void clearRecords();

protected:
    bool loadTable();
    virtual bool createTable();

    bool tableInitialized;
    QString tableName;
    QString lookupKey;
    QList<SchemaField> schema;
    QHash<QString, Record> records; // runtime copy of records, might want to get rid of it

    QSqlDatabase db;
};

#endif // DSTABLE_H
