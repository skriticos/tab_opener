tab\_opener
===========

tab\_opener is an application for dual mode file opening, quick command
execution and quick notes with the aim of optimizing the workflow with a small
set of filetypes.

It has been created to optimize my daily workflow with files and commands.
As such, it might not be useful for everyone, but some folks might appreciate
it.

Target audience
---------------

Users and developers who work with different applicaitons of files and want to
be able to quickly open them to optimize their workflow.


Technical Requirements
----------------------

A Linux desktop and Qt5 libraries installed. The application only uses the Qt
libraries, so no more requirements.


Screenshot
----------

![Alt text](/screenshot.jpeg?raw=true "Optional Title")



Video explaining the application
--------------------------------

[Youtube Video (https://www.youtube.com/watch?v=61jXPoaKjsM)]


Usecase
-------

The user is editing HTML files without IDE. He quickly switches between the
web-browser and the editor for the files. Because there are many files, the user
is hunting for them in the file manager. Particulary he is hunting for the
secondary application in the context menu as simple activation can only be bound
to one application.

With tab\_opener the user can configure two actions that are bound to quickly
accessable buttons. He can now quickly open the HTML file for editing and
viewing. He can also quickly open recent files the same way with the file
history widget.

At the same time the user can edit quick notes that are specific to eath file,
for example a qick todo list.

With tab\_opener the user does not have many windows open. Every time a file
is opened, the tab\_opener application closes. The user likes to Alt-Tab between
his application and he does not like it when a lot of auxilliary applications
clutter that list. tab\_opener does not. It will restore the last state when
started again, so the user will be able to continue his navigation and file
opening rigth where he left off.

He also likes to execute a few commands and tab\_opener gives him the
possibility to do so without needing to open a separate terminal. It will stay
open until the process completes execution and will present the user the output.
He can also make notes for each command he enters for later refference.

Finally the user is using git. tab\_opener provides three quick controls for
git usage: the user can pull, add+commit and push git repositories in the
current selection.

In combination these features make the workflow of the user much faster.


Version History
---------------

__v2.0: 2014-05-29: refactored, robust maintainable release__

- new feature: notes section selection: golbal, file and command specific notes
- new feature: simple git workflow integration (pull, add+commit, push)
  the executed commands and the result is shown in the command output widget
- usability improments: the application now remmembers the last selected folder,
  file, notes selection and command state it was in when it exited and restores
  this state on startup.
- usability improments: application stays around when a command is executed
  and the user opens files. the command widget execute button is now dual
  mode: when no command is running it starts the currently entered command
  in the currently selected folder. When a command is running, the icon
  changes to a fluctuating stop symbol and the user can terminate running
  commands.
- usability improvements: git control buttons disabled when a command is running
- usability improvemests: file open / copy buttons now disabled when no
  appropriate file is sellected
- usability improvmentes: selections are now updated in realtime across
  widgets
- testing integration. when run with the --test parameter, the application
  runs all test cases.
- fully refactored, maintainable code
- updated source tree, sensible file organization
  - separation of button widgets, module widgets and other sorce files for
	overview
- communication between modules is now almost entirely signal/slot based
  this makes it testable, easily debuggable and avoids side effect bugs
- database backend communication has been encapsulated in a transparent
  database table class. raw SQL queries are now minimal through the entire
  code. SQLite is used as a key -> values lookup table
- compilation now requires c++11 capable compiler and version Qt >= 5.0.0


__1.0: 2014-05-06: first working prototype__

- open files with two modes
- global notes
- execute commands and see return values
- preset list (folber bookmarks)
- file history (with recent and popular commands)
- command history
- lots of bugs
- no architecture (prototype)


Development
-----------

The application is written in Qt with QtCreator.
It depends on C++11 and Qt version >= 5.0.0.

The MainWindow module is the root application window. It does some minimal
state initialization and mostly connects all module signals and slots in the
application. If you want to know which modules talk with another this is the
place to go.

The DataCore module is the database backend. It's responsible for storing all
the data in the SQLite database and notifying the modules when data changes
occur. The actual database commulication is performed in the DsTable / DsTable
classes. These abstract database tables to key -> values entries. This minimizes
raw SQL queries in the code and possible bugs.

The defines.h file defines some placeholders for string literals used for
database commulication (mostly table culumn header names) to avoid typos and
some data structures that are shared between modules through signals.

The files in the *widgets* subfolder represent one of the major GUI modules:
file browser (top left), notes (top right), commands (middle right), presets
(bottom left) and the two history widgets (bottom right).

The *buttons* subfolder contains the various buttons used through the
application. Mostly signal enhanced QPushButtons. The CmDExecButton is a
somewhat bigger class with icon animation.


Roadmap
-------

v3.0

* verify paths on configuration and load
* improved error feedback (replace Q\_ASSERTS with user popups)
* improve test coverage
* add time tracker
* add file preview option
* add git gui and gitk shorthands
* make the history widgets optionally preset specific
* add user configurable commands


License
-------

GPLv2

