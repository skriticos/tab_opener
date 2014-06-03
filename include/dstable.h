#ifndef DSTABLE_H
#define DSTABLE_H

#include <QtCore>
#include <QtDebug>
#include <QtSql>

class DsTable : public QObject
{
    Q_OBJECT

public:
    typedef QHash<QString, QVariant> Record;

    enum FieldType { TEXT, INTEGER, REAL, BOOLEAN, BLOB };

    struct SchemaField {
        QString fieldName;
        DsTable::FieldType fieldType;
    };

    explicit DsTable(
        QString tableName,
        QList<SchemaField> fieldSchema,
        QSqlDatabase db,
        QObject *parent = 0);
    virtual ~DsTable();

    bool contains(QString lookupKey);
    void deleteRecord(QString lKey);
    virtual void insertRecord(Record record);
    int size();
    QStringList getRecordKeys();
    Record getRecord(QString lookupKey);
    void clearRecords();

signals:
    void sigRecordLookupFailure(QString lookupKey, QString tableName);

protected:
    void _loadTable();
    void _createTable();

    QString tableName;
    QString lookupKey;
    QList<SchemaField> schema;

    QSqlDatabase db;
};

#endif // DSTABLE_H
