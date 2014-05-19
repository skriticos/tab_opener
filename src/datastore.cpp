#include "datastore.h"

DataStore::DataStore(QObject *parent) : QObject(parent)
{
    this->dsDB = QSqlDatabase::addDatabase("QSQLITE");
    this->dsDB.setDatabaseName(QDir::homePath() + QDir::separator() + ".tab_opener.db");
    this->dsDB.open();

    // tblGeneral setup
    DsTable::SchemaField generalKey = {"gkey", DsTable::TEXT}; // general lookup key
    DsTable::SchemaField generalVal = {"gval", DsTable::TEXT}; // general value
    QList<DsTable::SchemaField> generalSchema; generalSchema << generalKey << generalVal;

    this->tblGeneral = new DsTable(this);
    this->tblGeneral->initTable("tblGeneral", generalSchema, this->dsDB);

    // tblExtension setup
    DsTable::SchemaField extStr    = {"ext_str", DsTable::TEXT};     // extension string
    DsTable::SchemaField extActPri = {"ext_act_pri", DsTable::TEXT}; // extension primary action
    DsTable::SchemaField extActSec = {"ext_act_sec", DsTable::TEXT}; // extension secondary action
    QList<DsTable::SchemaField> extSchema; extSchema << extStr << extActPri << extActSec;

    this->tblExtensions = new DsTable(this);
    this->tblExtensions->initTable("tblExtensions", extSchema, this->dsDB);

    // tblFiles setup
    DsTable::SchemaField filePath  = {"path", DsTable::TEXT};
    DsTable::SchemaField fileNotes = {"notes", DsTable::TEXT};
    QList<DsTable::SchemaField> fileSchema;
    fileSchema << filePath << fileNotes;

    this->tblFiles = new DsTableFav(this);
    this->tblFiles->initTable("tblFiles", fileSchema, this->dsDB);

    // tblCommands setup
    DsTable::SchemaField cmdCommand = {"command", DsTable::TEXT};
    DsTable::SchemaField cmdPath    = {"working_directory", DsTable::TEXT};
    DsTable::SchemaField cmdNotes   = {"notes", DsTable::TEXT};
    QList<DsTable::SchemaField> cmdSchema;
    cmdSchema << cmdCommand << cmdPath << cmdNotes;

    this->tblCommands = new DsTableFav(this);
    this->tblCommands->initTable("tblCommands", cmdSchema, this->dsDB);
}

DataStore::~DataStore()
{
    this->dsDB.close();
}

bool DataStore::setCommand(QString cmd, QString path, QString notes)
{
    DsTable::Record record;
    record.insert("command", cmd);
    record.insert("path", path);
    record.insert("notes", notes);
    return tblCommands->insertRecord(record);
}

QString DataStore::getGeneralValue(QString key)
{
    if(tblGeneral->contains(key))
        return tblGeneral->getRecord(key).value("gval").toString();
    return QString();
}

bool DataStore::setGeneralValue(QString key, QString value)
{
    DsTable::Record record;
    record.insert("gkey", key);
    record.insert("gval", value);
    return tblGeneral->insertRecord(record);
}

bool DataStore::setPreset(int pos, QString path) {
    QString presetKey = "preset" + QString::number(pos);
    return setGeneralValue(presetKey, path);
}

QString DataStore::getPreset(int pos)
{
    QString presetKey = "preset" + QString::number(pos);
    return getGeneralValue(presetKey);
}

bool DataStore::setExtensionValues(QString extStr, QString extActPri, QString extActSec)
{
    DsTable::Record record;
    record.insert("ext_str", extStr);
    record.insert("ext_act_pri", extActPri);
    record.insert("ext_act_sec", extActSec);
    return tblExtensions->insertRecord(record);
}

bool DataStore::setFile(QString path, QString notes)
{
    DsTable::Record record;
    record.insert("path", path);
    record.insert("notes", notes);
    return tblFiles->insertRecord(record);
}
