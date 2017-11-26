#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QProcess>
#include <QFileDialog>
#include <QStandardPaths>
#include <QCloseEvent>
#include <QMutex>
#include <QMimeData>
#include <QCoreApplication>
#include <QTranslator>
#include "ltfsdefine.h"


#define TAB_MOUNT_INDEX    0
#define TAB_FORMAT_INDEX   1
#define TAB_UNMOUNT_INDEX  2

#define MOUNT_TIMERTICK 5000
#define PROCESSING_TIMERTICK 1000

#define LTFSLSDEV_CMD  "ltfslsdev.exe"
#define UMOUNT_CMD "/sbin/umount"

#define UMOUNT_OPT_F "-f"

#define TRANSLATE_RESOURCE_NAMEBASE ":/qm"

namespace Ui{
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void postprocess_stdout_output();
    void postprocess_stderr_output();
    void lsdevprocess_stdout_output();
    void mountprocess_stdout_output();
    
    void process_exit(int, QProcess::ExitStatus);
    void on_pushButton_Execute_clicked();
    void on_tabWidget_Comannds_currentChanged(int index);
    
private:
    Ui::MainWindow *ui;
    
    //QApplicationインスタンス
	QCoreApplication  *main;
	QTranslator		  *translater;

    QLabel status_label;
    QRegExp serialreg;
    QValidator *serialval;
    QProcess post_process;

    QProcess lsdev_process;
    QProcess mount_process;

    QRegExp *ltfsmount_val;
    QRegExp *ltfsscsiid_val;

    QStringList hlabels;
    int         mw_TimerId;
    int         processing_TimerId;
    QMutex      umount_mutex;
    QDir        mount_homedir;
    int         dotcount;

    int         cur_commandindex; //Current Execute command Tab index
    bool        ismounted;
    QString     cur_translabel;

    //label pattern name.
    //Depending on your rules, it is information to customize.
    //Reference ＝ YYYYNNL6 or YYYYNNL7 like.
    QString     label_prefix;    //YYYY Ex. 2017,2018
    int         label_min;       //Start value of NN for LTFS format command batch ex. 1
    int         label_max;       //End value of NN for LTFS format command batch   ex. 8
    QString     label_ltogen;    //L6 or L7 or L8....
    bool        label_redirect;
    int         format_index;    //current format target number(Incremantal)
    int         format_endindex; //current format end index.

    bool setArgsbyIndex(int tab_index,QString *cmdname_pt,QStringList *args_pt);
    void setEnabled(bool enabled);
    void getLTFSlsdevList();
    void getLTFSmountList();
    void setLTFSCurrentProcessing();
    bool parseLabel(QString labelname);
    
    int  getSCSIDeviceInfo(struct tc_drive_info *buf, int count);
    
    void timerEvent(QTimerEvent *timer_event);
    void closeEvent(QCloseEvent *event);

};



#endif // MAINWINDOW_H
