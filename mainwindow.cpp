#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    if(QLocale::system().language() == QLocale::Japanese){
        translater =  new QTranslator();
        main = QApplication::instance();
        
        translater->load("LTFSLEManager",
                         TRANSLATE_RESOURCE_NAMEBASE);
        main->installTranslator(translater);
    }

    ui->setupUi(this);

    this->setAcceptDrops(true);
    ui->tableWidget_mountList->setColumnCount(5);
    hlabels << tr("Label");
    hlabels << tr("Location");
    hlabels << tr("Capacity");
    hlabels << tr("Remaining");
    hlabels << tr("Status");

    ui->tableWidget_mountList->setHorizontalHeaderLabels(hlabels);
    ui->tableWidget_mountList->setRowCount(LTOLOADER_NUMBER_MAX);

    QHeaderView* hheader = ui->tableWidget_mountList->horizontalHeader();
    hheader->setSectionResizeMode(QHeaderView::ResizeToContents);

    serialreg.setPattern("([A-Z]|[0-9]){6}");
    serialval = new QRegExpValidator(serialreg,this);
    ui->statusBar->setLayoutDirection(Qt::RightToLeft);
    ui->statusBar->addPermanentWidget(&status_label);
    mw_TimerId =  0;
    processing_TimerId = 0;
    dotcount = 0;
    ltfsmount_val = new QRegExp(LTFSLE_MOUNT_REG_PATTERN);
    ltfsscsiid_val = new QRegExp(LTFSLE_SCSIID_REG_PATTERN);

    label_prefix.clear();
    label_min = 99;
    label_max = 1;
    label_ltogen.clear();
    label_redirect = false;
    ui->comboBox_min->clear();
    ui->comboBox_max->clear();
    format_index = 0;
    format_endindex = 0;
    cur_commandindex = 0;

    QObject::connect(&lsdev_process,SIGNAL(readyReadStandardOutput()),
                         this, SLOT(lsdevprocess_stdout_output()));
    getLTFSlsdevList();

    QObject::connect(&mount_process,SIGNAL(readyReadStandardOutput()),
                         this, SLOT(mountprocess_stdout_output()));

    mw_TimerId = QObject::startTimer(MOUNT_TIMERTICK,Qt::CoarseTimer);
    processing_TimerId = QObject::startTimer(PROCESSING_TIMERTICK,Qt::CoarseTimer);

    QObject::connect(&post_process,SIGNAL(readyReadStandardOutput()), this, SLOT(postprocess_stdout_output()));
    QObject::connect(&post_process,SIGNAL(readyReadStandardError()), this, SLOT(postprocess_stderr_output()));
    QObject::connect(&post_process,SIGNAL(finished(int, QProcess::ExitStatus)),
					 this,SLOT(process_exit(int, QProcess::ExitStatus)));
    if(ismounted){
        getLTFSmountList();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::setArgsbyIndex(int tab_index, QString *cmdname_pt, QStringList *args_pt){

    bool ret = true;
    QString target_label;
    QString volumename;
    target_label.clear();
    volumename.clear();

    QDir ltfswkdir(ui->lineEdit_workdir->text());
    QString wkdirname(ltfswkdir.dirName());

    switch(tab_index){

        case TAB_FORMAT_INDEX:
            //qDebug() << TAB_FORMAT_INDEX;
            cmdname_pt->append(LTFS_COMMAND_PYTHON);
            args_pt->append(LTFS_COMMAND_ADMINTOOL);
            //args_pt->append(ui->comboBox_DriveID->currentText());
            args_pt->append(LTFS_COMMAND_ARGS_T);

            if(format_index == 0){
                QMessageBox msgbox;
                msgbox.setStandardButtons(QMessageBox::Yes);
                msgbox.setIcon(QMessageBox::Critical);


                if(ui->comboBox_max->currentText().toInt() < ui->comboBox_min->currentText().toInt()){
                    msgbox.setText(tr("Incorrect values of min and max."));
                    msgbox.exec();
                    return(false);
                }
                else if(ui->lineEdit_Volumename->text().isEmpty()){
                    msgbox.setText(tr("Input Volume name."));
                    msgbox.exec();
                    return(false);
                }
                format_index = ui->comboBox_min->currentText().toInt();
                format_endindex = ui->comboBox_max->currentText().toInt();
            }
            else{
                format_index++;
            }
            target_label.append(label_prefix);
            target_label.append(QString::number(format_index).rightJustified(2,'0')); //201701
            target_label.append(label_ltogen);
            args_pt->append(target_label);
            args_pt->append(LTFS_COMMAND_ARGS_F);
            args_pt->append(LTFS_COMMAND_ARGS_HYPHENHYPEHN);
            args_pt->append(LTFS_COMMAND_ARGS_N);
            volumename.append(ui->lineEdit_Volumename->text() + "_Volume" + QString::number(format_index));
            args_pt->append(volumename);
            args_pt->append(LTFS_COMMAND_ARGS_F);
            ui->textEdit_Log->append(tr("Label:") + target_label + tr(" Volume:") + volumename + tr("  formatting."));

            //qDebug() << *cmdname_pt;
            //qDebug() << *args_pt;

            break;

        case TAB_MOUNT_INDEX:

            //qDebug() << TAB_MOUNT_INDEX;
            cmdname_pt->append(LTFS_COMMAND_LTFS);
            args_pt->append(LTFS_COMMNAD_LTFS_ARGS_O);
            args_pt->append(LTFS_COMMNAD_LTFS_ARGS_O_DEVNAME + ui->label_DriveAddress_cur->text());

            args_pt->append(LTFS_COMMNAD_LTFS_ARGS_O);
            if(ui->lineEdit_synctime->text() == "0" || ui->lineEdit_synctime->text().isEmpty()){
                args_pt->append((QString)LTFS_COMMNAD_LTFS_ARGS_O_SYNCTYPE + (QString)LTFS_COMMNAD_LTFS_ARGS_O_UMOUNTSYNC);
            }
            else{
                args_pt->append((QString)LTFS_COMMNAD_LTFS_ARGS_O_SYNCTYPE + (QString)LTFS_COMMNAD_LTFS_ARGS_O_SYNCTIME + ui->lineEdit_synctime->text());
            }

            //Defaults LTFSLE workdir set to D:\LTFS_LE_WKDIR.
            //If you want to change default work dir, change mainwindow.ui with QtDesigner.
            args_pt->append(LTFS_COMMNAD_LTFS_ARGS_O);
            args_pt->append(LTFS_COMMNAD_LTFS_ARGS_O_WORKDIR + ui->lineEdit_workdir->text());

            //Defaults Driveletter is P Drive. If you want to change drive letter.
            //Modify mainwindow.ui or comboBox_assigndrive property.
            args_pt->append(ui->comboBox_assigndrive->currentText());

            //Delete LTFSLE work directory.
            //Note:
            //If you can save the uniqueness barcode label,
            //you may not need to delete the folder.
            //In my environment, the barcode is not unique.
            //So delete the cache directory every time.
            if(ltfswkdir.exists()){
                ltfswkdir.removeRecursively();
                ltfswkdir.cdUp();
                ltfswkdir.mkdir(wkdirname);
            }
            else{
                ltfswkdir.cdUp();
                ltfswkdir.mkpath(wkdirname);
            }
            break;

        case TAB_UNMOUNT_INDEX:

            QObject::killTimer(mw_TimerId);

            // Is tape in the drive to unmount?
            // In the LTFS LE environment, Unmounting isnt automatically remove the cartridge in the drive.
            // Therefore, start eject command myself.
            if(!cur_translabel.isEmpty()){
                // eject and goto home slot.
                cmdname_pt->append(LTFS_COMMAND_PYTHON);
                args_pt->append(LTFS_COMMAND_ADMINTOOL);
                args_pt->append(LTFS_COMMAND_ARGS_T);
                args_pt->append(cur_translabel);
                args_pt->append(LTFS_COMMAND_ARGS_M);
                args_pt->append(LTFS_COMMAND_ARGS_HOME);
                ui->textEdit_Log->append("LTFSLEManager:" + cur_translabel + tr("Ejecting.."));
                //qDebug() << "ejectmove:" << *cmdname_pt << *args_pt;
            }
            else{
                cmdname_pt->append(LTFS_COMMAND_LTFSUNMOUNT);
                args_pt->append(ui->label_DriveLetter_cur->text());
            }
            break;
        /*
        case TAB_RECOVERY_INDEX:
            no support.
        */
        default:
            ret = false;
            break;
    }
    return ret;
}

void MainWindow::setEnabled(bool enabled){
    ui->tabWidget_Comannds->setEnabled(enabled);
    ui->pushButton_Execute->setEnabled(enabled);
}

void MainWindow::getLTFSlsdevList(){
    if(lsdev_process.state() == QProcess::NotRunning){
        lsdev_process.start(LTFSLSDEV_CMD);
    }
}

void MainWindow::getLTFSmountList(){
    if(mount_process.state() == QProcess::NotRunning){
        QString cmd;
        QStringList args;
        cmd.append(LTFS_COMMAND_PYTHON);
        args.append(LTFS_COMMAND_ADMINTOOL);
        args.append(LTFS_COMMAND_ARGS_I);
        //qDebug() << cmd;
        //qDebug() << args;
        mount_process.start(cmd,args);
        //qDebug() << mount_process.state();
    }
}

void MainWindow::setLTFSCurrentProcessing(){
    if(post_process.state() == QProcess::Running){
        QString processing = tr("processing");
        //qDebug() << dotcount;
        for(int i=dotcount;i > 0;i--){
            processing.append(".");
        }
        dotcount++;
        if(dotcount > 3){
            dotcount=0;
        }
        status_label.setText(processing);
    }
}


void MainWindow::postprocess_stdout_output(){
    QByteArray readstdout = post_process.readAllStandardOutput();
    if(!readstdout.isEmpty() && !readstdout.isNull()){
        ui->textEdit_Log->append(QString(readstdout).trimmed());
        ui->textEdit_Log->textCursor().movePosition(QTextCursor::End);
    }
}

void MainWindow::postprocess_stderr_output(){
    QByteArray readstderrout = post_process.readAllStandardError();
    if(!readstderrout.isEmpty() && !readstderrout.isNull()){
        ui->textEdit_Log->append(readstderrout);
        ui->textEdit_Log->textCursor().movePosition(QTextCursor::End);
    }
}

void MainWindow::lsdevprocess_stdout_output(){
    
    bool work_mounted = false;
    //Exclusive lock for umount execution and table info referesing.
    umount_mutex.lock();

    ismounted = false;
    ui->tabWidget_Comannds->setTabEnabled(TAB_FORMAT_INDEX,false);
    ui->tabWidget_Comannds->setTabEnabled(TAB_MOUNT_INDEX,true);
    ui->tabWidget_Comannds->setTabEnabled(TAB_UNMOUNT_INDEX,false);

    QTextStream stream(lsdev_process.readAllStandardOutput());
    QString line = stream.readLine();
    //qDebug() << line;
    while(!line.isNull()){
        //qDebug() << line;
        //Separate appropriately with spaces.
        //Mount options in the back are disjointed but I will not mind:)
        QStringList list = line.split(" ",QString::SkipEmptyParts);

        for(int i=0;i < list.size();i++){
            //Does the pattern of the first element match "[A-Z]" ?
            //Already mounted driveletter?
            if(ltfsmount_val->exactMatch(list.at(i))){
                //mounted flag on
                work_mounted = true;
                ui->tabWidget_Comannds->setTabEnabled(TAB_FORMAT_INDEX,true);
                ui->tabWidget_Comannds->setTabEnabled(TAB_MOUNT_INDEX,false);
                ui->tabWidget_Comannds->setTabEnabled(TAB_UNMOUNT_INDEX,true);
                ui->label_DriveLetter_cur->setText(list.at(i));
            }
            else if(ltfsscsiid_val->exactMatch(list.at(i))){
                ui->label_DriveAddress_cur->setText(list.at(i));
            }
        }
        line = stream.readLine();
    }
    if(work_mounted){
        ismounted = true;
        if(ui->tabWidget_Comannds->currentIndex() == TAB_MOUNT_INDEX){
            ui->tabWidget_Comannds->setCurrentIndex(TAB_UNMOUNT_INDEX);
        }
    }
    else{
        ismounted = false;
        ui->label_DriveLetter_cur->setText(tr("No mounted!!"));
        if(ui->tabWidget_Comannds->currentIndex() != TAB_MOUNT_INDEX){
            ui->tabWidget_Comannds->setCurrentIndex(TAB_MOUNT_INDEX);
        }
    }
    umount_mutex.unlock();
}

void MainWindow::mountprocess_stdout_output(){

    int rowcount = 0;
    umount_mutex.lock();

    ui->tableWidget_mountList->clearContents();
    ui->tableWidget_mountList->setRowCount(8);
    QTextStream stream(mount_process.readAllStandardOutput());
    QString line = stream.readLine();

    //Sample of the contents of the line like this. Perhaps based on csv.
    //201706L7 -> Location: Data transfer element,    Address:  257, Capacity:   5338GB, Remaining:   5338GB, Status: Valid LTFS

    while(!line.isNull()){
        //parse comma
        QStringList list = line.split(",",QString::SkipEmptyParts);
        QStringList lable_loc = ((QString)list.at(0)).split("->",QString::SkipEmptyParts);
        QString label_trimmed = QString(lable_loc.at(0)).trimmed();
        parseLabel(label_trimmed);

        ui->tableWidget_mountList->setItem(rowcount,0,new QTableWidgetItem(label_trimmed));

        QString loc = QString(QString(lable_loc.at(1)).split(":",QString::SkipEmptyParts).at(1)).trimmed();
        if(loc.compare("Data transfer element") == 0){
            cur_translabel = label_trimmed;
            //qDebug() << "cur_changed:" << cur_translabel;
        }


        ui->tableWidget_mountList->setItem(rowcount,1,new QTableWidgetItem(loc)); // Medium storage element

        QString capa = QString(list.at(2)).split(":",QString::SkipEmptyParts).at(1);
        ui->tableWidget_mountList->setItem(rowcount,2,new QTableWidgetItem(capa.trimmed()));//5338GB

        QString remain = QString(list.at(3)).split(":",QString::SkipEmptyParts).at(1);
        ui->tableWidget_mountList->setItem(rowcount,3,new QTableWidgetItem(remain.trimmed())); //3338GB

        QString status = QString(list.at(4)).split(":",QString::SkipEmptyParts).at(1);
        ui->tableWidget_mountList->setItem(rowcount,4,new QTableWidgetItem(status.trimmed()));    //Status: Valid LTFS
        rowcount++;
        list.clear();
        line = stream.readLine();
    }
    //Asc sort
    ui->tableWidget_mountList->sortItems(0);
    if(!label_redirect){
        for(int i=label_min; i <= label_max;i++){
            ui->comboBox_min->addItem(QString::number(i));
            ui->comboBox_max->addItem(QString::number(i));
        }
        ui->comboBox_max->setCurrentIndex(label_max - 1);
        label_redirect = true;
    }

    umount_mutex.unlock();
}

bool MainWindow::parseLabel(QString labelname){
    bool ret = true;
    //qDebug() << labelname.mid(4,2);
    int work_int = labelname.mid(4,2).toInt();
    label_prefix = labelname.left(4);  //2017
    //qDebug() << work_int;
    if(work_int < label_min){
        label_min = work_int;
    }
    if(work_int > label_max){
        label_max = work_int;
    }
    label_ltogen = labelname.mid(6);
    return ret;
}

void MainWindow::process_exit(int exitCode, QProcess::ExitStatus status){
    /*
    Some LTFS commands do not correctly return the return code,
    so you can not notify the status correctly. .
    This is not very good implementation... :(
    */
    
    bool is_normalexit = exitCode == 0 ? true:false;
    bool next_trigger = false;
    QString message;
    message.clear();

    //qDebug() << ui->tabWidget_Comannds->currentIndex();
    switch(cur_commandindex){
        case TAB_FORMAT_INDEX:
            if(is_normalexit){
                message = tr("Format Success.");
                if(format_index < format_endindex){
                    next_trigger = true;
                }
                else{
                    format_index = 0;
                    format_endindex = 0;
                }
            }
            else{
                message = tr("Format Failed or finished. Check Log");
            }

            break;
        case TAB_MOUNT_INDEX:
            if(is_normalexit){
                message = tr("Mount success");
                label_prefix.clear();
                label_min = 99;
                label_max = 1;
                label_ltogen.clear();
                label_redirect = false;
                ui->comboBox_min->clear();
                ui->comboBox_max->clear();
                format_index = 0;
                format_endindex = 0;
            }
            else{
                message = tr("Mount Failed. Check log");
            }
            break;
        case TAB_UNMOUNT_INDEX:
            if(!cur_translabel.isEmpty()){
                if(is_normalexit){
                    message = tr("Eject Success.");
                    cur_translabel.clear();
                    next_trigger = true;
                }
                else{
                    message = tr("Eject failed. Check log");
                }
            }
            else{
                if(is_normalexit){
                    message = tr("UnMount Success.");
                    ismounted = false;
                    ui->tableWidget_mountList->clearContents();
                }
                else{
                    message = tr("UnMount Failed Check log");
                }
            }
            mw_TimerId = QObject::startTimer(MOUNT_TIMERTICK,Qt::CoarseTimer);
            break;
    }
    status_label.setText(message);
    message.prepend("LTFSLEManager:");
    ui->textEdit_Log->append(message);
    //qDebug() << message;
    if(ismounted){
        getLTFSmountList();
    }
    else{
        getLTFSlsdevList();
    }
    setEnabled(true);
    if(next_trigger){
        //続いてのコマンドを勝手に実行するよ。
        //次に必要な準備をしてからこいつ呼ぶのよ
        on_pushButton_Execute_clicked();
    }
}

void MainWindow::on_pushButton_Execute_clicked()
{

    QString command_name;
    QStringList args;
    
    if(!setArgsbyIndex(ui->tabWidget_Comannds->currentIndex(),&command_name,&args)){
        //qDebug() << "Error occured. check LTFS Environment";
        return;
    }
    cur_commandindex = ui->tabWidget_Comannds->currentIndex();

	setEnabled(false);
	post_process.start(command_name,args);
}

void MainWindow::timerEvent(QTimerEvent *timer_event){

    if(timer_event->timerId() == mw_TimerId){
        if(!ismounted){
            getLTFSlsdevList();
        }
        else{
            getLTFSmountList();
        }
    }
    else if(timer_event->timerId() == processing_TimerId){
        setLTFSCurrentProcessing();
    }
}

void MainWindow::closeEvent(QCloseEvent *event){

    if(post_process.state() == QProcess::Running){
        QMessageBox msgbox;
        msgbox.setStandardButtons(QMessageBox::Yes);
        msgbox.setIcon(QMessageBox::Warning);
        msgbox.setText(tr("command is running. Please wait a while."));
        if(msgbox.exec() == QMessageBox::Yes){
            event->ignore();
        }
    }
}

void MainWindow::on_tabWidget_Comannds_currentChanged(int index)
{
    //qDebug() << "on_tabWidget_Comannds_currentChanged" << index;
    if(ismounted){
        ui->tabWidget_Comannds->setTabEnabled(TAB_FORMAT_INDEX,true);
        ui->tabWidget_Comannds->setTabEnabled(TAB_MOUNT_INDEX,false);
        ui->tabWidget_Comannds->setTabEnabled(TAB_UNMOUNT_INDEX,true);
        if(ui->tabWidget_Comannds->currentIndex() == TAB_MOUNT_INDEX){
            ui->tabWidget_Comannds->setCurrentIndex(TAB_UNMOUNT_INDEX);
        }
    }
    else{
        ui->tabWidget_Comannds->setTabEnabled(TAB_FORMAT_INDEX,false);
        ui->tabWidget_Comannds->setTabEnabled(TAB_MOUNT_INDEX,true);
        ui->tabWidget_Comannds->setTabEnabled(TAB_UNMOUNT_INDEX,false);
        if(ui->tabWidget_Comannds->currentIndex() != TAB_MOUNT_INDEX){
            ui->tabWidget_Comannds->setCurrentIndex(TAB_MOUNT_INDEX);
        }
    }
    QString statictext = tr("Execute");
    switch(ui->tabWidget_Comannds->currentIndex()){
        case TAB_FORMAT_INDEX:
            ui->pushButton_Execute->setText(tr("Format ") + statictext);
            break;
        case TAB_MOUNT_INDEX:
            ui->pushButton_Execute->setText(tr("Mount ") + statictext);

            break;
        case TAB_UNMOUNT_INDEX:
            ui->pushButton_Execute->setText(tr("UnMount ") + statictext);
            break;
        default:
            break;
    }
}
