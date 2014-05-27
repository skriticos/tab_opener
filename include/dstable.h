#ifndef DSTABLE_H
#define DSTABLE_H

#include <QtCore>
#include <QtDebug>
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

    explicit DsTable(QObject *parent = 0);
    virtual ~DsTable();

    virtual void initTable(QString            tableName,
                   QList<SchemaField> fieldSchema,
                   QSqlDatabase       db);

    bool contains(QString lookupKey);
    void deleteRecord(QString lKey);
    virtual void insertRecord(Record record);
    int size();
    QStringList getRecordKeys();
    Record getRecord(QString lookupKey);
    void clearRecords();

protected:
    void _loadTable();
    void _createTable();

    bool tableInitialized;
    QString tableName;
    QString lookupKey;
    QList<SchemaField> schema;
    QHash<QString, Record> records; // runtime copy of records, might want to get rid of it

    QSqlDatabase db;
};

#endif // DSTABLE_H
