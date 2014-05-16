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
    bool insertRecord(Record record);
    int size();
    QStringList getRecordKeys();
    Record getRecord(QString lookupKey);
    void clearRecords();

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
