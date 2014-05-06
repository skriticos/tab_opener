tab\_opener
==========

tab\_opener is an aplication that focuses on the thing that you do most of the
time in a file manager: open files. It is aimed at the developer type of person
who want's to quickly navigate projects and cares a great deal about optimizing
the workflow for a hand full of filetypes.


Screenshot
----------

![Alt text](/snapshot1.jpg?raw=true "Optional Title")


Video explaining the application
--------------------------------

[Youtube Video (https://www.youtube.com/watch?v=BPbSBt38CBk)]

Features on a glace
-------------------

* two configurable primary actions for filetypes

  * the idea is that you can access two opening modi for a filetype instead of
	only one like in traditional file managers and hunt for the other in the
	context menu

* stays out of your face when you don't need it

  * each time you open a file with tab\_opener, it closes it's self. This way it
	is not cluttering your alt-tab / currently running application list. It's a
	light weight application, so you can quickly start it again the next time
	you need it. No more 5 file browsers hanging around.

* quickly execute commands without the need for a separate terminal

  * execute commands in the currently selected path. show stdout, stderr
	(highlighted) and return code

* quick access to recently used files and commands

  * the 10 recently used files and comannds are just one click away

* quick access to most used files and commands

  * same as with recently used files and commands, just for usage count

* file specific notes (work in progress)

  * currrently the notes widget is just one static wall, but the next feature
	will be file type specific notes

* sqlite database backend

  * in case you want to save / edit the saved data, you can do it with a sqlite
	editor. no obsure binary blob save files. the application database file can
	be found in the home folder and is called .tab_opener.db


Development
-----------

The application is written in Qt with QtCreator. It has been sucessfully
compiled with Qt 4.8.x and Qt 5.2.x.

Most things happen in the MainWindow class.

Other structural elements:

* datastore

  * contains all the data that is stored to database
  * stores and loads data to/from database

* configwidget

  * widget to configure file extensions, presets and prefered terminal / file
	manager

* filelist, cmdlist

  * widgets for recent and popular files and commands (lower widgets)

* hoverbutton

  * baseclass for most buttons in the application. QPushButton that is flat when
	mouse is not over them.

* presetbutton

  * button for presets (lower left part)

* charmbutton

  * button for breadcrumb / charm navigation in the top left part of the main
	window

* filebutton, commandbutton

  * buttons on the filelist, cmdlist widgets


Roadmap
-------

* make notes file specific
* rememmber last location on close and restore on open
* layout currently is designed for 1080p, should be more flexible
* disable control elements when no file is selected
* properly sort recent files / commands (last file / command should be first,
  not just in the list)
* might want to have preset specific recent files / commands ?
* tweak terminal, combine quoted arguments
* simple git integration
* add a cheat sheet button


License
-------

GPLv2

