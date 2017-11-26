# LTFSLEManager
LTFS Library Edition management tool with Qt5.x

![LTFSLEManager](https://github.com/KengoSawa2/LTFSLEManager/blob/master/SS/LTFSLEManager.png "LTFSLEManager")

#### Overview
LTFSLEManager is LTFS Library Edition management tool.

LTFSLEManager provides the following functions.

- Mounting LTFSLE with GUI
- Batch format (amount of LTFS format at one time)
- Unmounting LTFSLE with GUI

All functions are implemented with the commands provided by LTFSLE.
That is, LTFSLEManager is front-end software for LTFSLE.

Before you get started,You may study IBM Spectrum Archive Library Edition.
https://www.ibm.com/support/knowledgecenter/STZMZN_2.1.7.1/ltfs_le_ichome.html

And LTFSLE command reference.
https://www.ibm.com/support/knowledgecenter/STZMZN_2.1.7.1/ltfs_managing_command_line_library_edition.html

#### License

Source Code license is [BSD 2-Clause License]  

#### Specification/Usage

Build LTFSLEManager.exe and DoubleClick. Good luck! :)

#### to Build
We checked Windows Server 2012R2 only.
Linux Version is not checked.

Qt Library(5.6) and QtCreator are necessary to make.
Start up QtCreator and read project file "LTFSLEManager.pro"
Qt Framework can download here.

https://www.qt.io/

#### Notice
This tool is an in-house optimized implementation.
If you want to use it in your company, you need to change the source code.
I think change is pretty easy, cheers!

#### TODO
- complex LTFS format batch?
- LTFSLE for Linux checking?

