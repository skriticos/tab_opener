#ifndef DEFINES_H
#define DEFINES_H

// string defines to avoid typos
// mostly database column headers
// also notes selection values
#define COMMAND "command"
#define CURRENT_COMMAND "current_command"
#define EXT_ACT_PRI "ext_act_pri"
#define EXT_ACT_SEC "ext_act_sec"
#define EXT_STR "ext_str"
#define GKEY "gkey"
#define GVAL "gval"
#define NOTE "note"
#define NOTES "notes"
#define PATH "path"
#define WORKING_DIRECTORY "working_directory"
#define LAST_NOTE_STATE "last_note_state"
#define SELECTED_FILE "selected_file"
#define FILE "file"
#define GLOBAL "global"
#define FILE_BROWSER "file_browser"
#define TERMINAL_EMULATOR "terminal_emulator"

namespace Config {

    struct ExtensionEntry {
        QString extName;        // exetnsion name, e.g. "txt"
        QString extActPri;      // primary action command, e.g. "/usr/bin/gvim --remote"
        QString extActSec;      // secondary action command
    };

}

#endif // DEFINES_H
