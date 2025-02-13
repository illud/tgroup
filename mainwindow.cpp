#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "groups.h"
#include "dbmanager.h"
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    QString folderPath = "shortcuts";  // Specify the folder name or path

    QDir dir;

    // Check if the folder already exists
    if (!dir.exists(folderPath)) {
        // Create the folder
        if (dir.mkpath(folderPath)) {
            qDebug() << "Folder created successfully!";
        } else {
            qDebug() << "Failed to create the folder!";
        }
    } else {
        qDebug() << "Folder already exists!";
    }


   DbManager *db = new DbManager(path);

    QVector<DbManager::Group> groups;
   groups = db->getGroups();

    ui->tableWidget->setRowCount(groups.count());
    for (int i = 0; i < groups.count(); ++i) {
       ui->tableWidget->setItem(i,0,new QTableWidgetItem(groups[i].groupName));
    }

   delete db;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_groupsBtn_clicked()
{
    DbManager *db = new DbManager(path);

    QVector<DbManager::Group> groups;
    groups = db->getGroups();

    ui->tableWidget->setRowCount(groups.count());
    for (int i = 0; i < groups.count(); ++i) {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(groups[i].groupName));
    }

    delete db;

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_newGroupBtn_clicked()
{
    //groups *gp = new groups(this);
    //gp->show();
    DbManager *db = new DbManager(path);

    QVector<DbManager::Group> groups;
    groups = db->getGroups();

    ui->tableWidget->setRowCount(groups.count());
    for (int i = 0; i < groups.count(); ++i) {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(groups[i].groupName));
    }


    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_searchGameExeBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Get App EXE");
    QDir d = QFileInfo(filePath).absoluteDir();
    QString absolute = d.absoluteFilePath(filePath);
   // data = absolute;
    data.append(absolute);

     ui->tableWidget_2->setRowCount(data.count());
    for (int i = 0; i < data.count(); ++i) {
         QFileInfo fileInfo(data[i]);
         QString fileName = fileInfo.fileName();  // Get the file name

         fileName.replace(".exe","");
        ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(fileName));
    }
}

void MainWindow::on_saveBtn_clicked()
{
    // Instance db conn
    DbManager *db = new DbManager(path);

    // Inser into games table
    QString result = QStringList::fromVector(data).join(", ");
    db->insertGroup(ui->lineEdit->text(), result);

    QString folderPath = "shortcuts";  // Folder where the shortcut will be created
    QString shortcutPath = folderPath + "/" + ui->lineEdit->text() + ".lnk"; // Shortcut name

    QString appDir = QCoreApplication::applicationDirPath();

    // Command to create a shortcut using PowerShell script
    QString command = QString("powershell -Command \"$WshShell = New-Object -ComObject WScript.Shell; "
                              "$shortcut = $WshShell.CreateShortcut('%1'); "
                              "$shortcut.TargetPath = '%2\\tgroup.exe'; "
                              "$shortcut.Arguments = '%3'; "
                              "$shortcut.Save()\"").arg(shortcutPath).arg(appDir).arg(ui->lineEdit->text());

    QProcess process;
    process.start(command);
    process.waitForFinished();

    if (process.exitCode() == 0) {
        qDebug() << "Shortcut created successfully!";
    } else {
        qDebug() << "Failed to create the shortcut!";
    }

    delete db;
}


void MainWindow::on_groupBtn_clicked()
{
    groups* gp = new groups(nullptr, "apps");
    gp->show();
}

