#ifndef DSTABLE_H
#define DSTABLE_H

#include <QtCore>
#include <QtSql>

class DsTable : public QObject{
    Q_OBJECT

public:
    typedef QHash<QString, QVariant> Record;

    enum FieldType { TEXT, INTEGER, REAL, BOOLEAN, BLOB };

    struct SchemaField {
        QString fieldName;
        DsTable::FieldType fieldType;
    };

    explicit DsTable(QString tableName,
                     QList<SchemaField> fieldSchema,
                     QSqlDatabase db,
                     QObject *parent = 0);

    bool recordExists(QString lookupKey);
    void clearRecords();
    bool insertRecord(Record record);
    int getRecordCount();
    Record getRecord(QString lookupKey);

private:
    void loadTable();
    void createTable();

    QString tableName;
    QString lookupKey;
    QList<SchemaField> schema;
    QHash<QString, Record> records;

    QSqlDatabase db;
};

#endif // DSTABLE_H
