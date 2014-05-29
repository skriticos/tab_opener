#ifndef HISTORY_H
#define HISTORY_H

namespace History
{
    struct Entry {
        QString filePath;
        QString commandString;
        QString workingDirecotry;
    };
    enum WidgetType {
        UNDEFINED,
        FILEHISTORY,
        COMMANDHISTORY
    };
}

#endif // HISTORY_H
