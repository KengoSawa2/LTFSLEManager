# LTFSLEManager
LTFS Library Edition management tool with Qt5.x

![LTFSLEManager](https://github.com/KengoSawa2/LTFSLEManager/blob/master/SS/LTFSLEManager.png "LTFSLEManager")

## Overview
LTFSLEManager is LTFS Library Edition management tool.

LTFSLEManager provides the following functions.

- Mounting LTFSLE with GUI
- Batch format (amount of LTFS format at one time)
- Unmounting LTFSLE with GUI

All functions are implemented with the commands provided by LTFSLE.
That is, LTFSLEManager is front end tool for LTFSLE.

Before you get started,You may study IBM Spectrum Archive Library Edition.
https://www.ibm.com/support/knowledgecenter/STZMZN_2.1.7.1/ltfs_le_ichome.html

And LTFSLE command reference.
https://www.ibm.com/support/knowledgecenter/STZMZN_2.1.7.1/ltfs_managing_command_line_library_edition.html

### License

Source Code license is [BSD 2-Clause License]  

### Usage

1. Install IBM Spectrum Archive(LTFS) Library Edition.
2. set LTFSLE commands path to "PATH" environment variable.
   (ex:C:\Program Files\IBM\LTFS)

Build LTFSLEManager.exe and DoubleClick. Good luck! :)

### Checked Environment

Software:
IBM Spectrum Archive(LTFS) Library Edition V2.1.7 for Windows (ILAN License)

Hardware:
TS2900 with LTO-7 drive
https://www.ibm.com/us-en/marketplace/ts2900

### to Build
We checked Windows Server 2012R2 only.
Linux Version is not checked.

This product need Qt Library(5.6) and QtCreator are necessary to make.
Start up QtCreator and read project file "LTFSLEManager.pro"
Qt Framework can download here.

https://www.qt.io/

## Notice
This tool is in-house optimized implementation.
If you want to use at your company, you need to change the source code.
I think change is pretty easy, cheers!

This tool is affected by the implementation of LTFSLE and commands outputs format.
Take care of your LTFS Environment.

#### TODO
- complex LTFS format batch?
- LTFSLE for Linux checking?

