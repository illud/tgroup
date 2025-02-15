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

    load_groups();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::load_groups(){
    DbManager *db = new DbManager(path);

    QVector<DbManager::Group> groups;
    groups = db->getGroups();

    int currentRow = 0;
    ui->tableWidget->setRowCount(groups.count());
    for (int i = 0; i < groups.count(); ++i) {
        ui->tableWidget->setColumnWidth(0, 230);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(groups[i].groupName));

        // CELL BUTTON
        QPushButton* button = new QPushButton();
        button->setText("X");
        button->setStyleSheet("QPushButton {    background-color: rgb(41, 98, 255);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;	border-radius: 20px;	border-style: outset;}QPushButton::hover{     background-color: rgb(33, 78, 203);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}QPushButton::focus:pressed{ 	background-color: rgb(38, 72, 184);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}");
        //Sets button property to identify button
        button->setProperty("btnDeleteId", groups[i].id);
        //Adds button to current index row
        ui->tableWidget->setCellWidget(currentRow, 1,  button);
        // Set the column width to fit the widget (e.g., 100 pixels)
        ui->tableWidget->setColumnWidth(1, 30);
        //emit connect(button,SIGNAL(clicked()),this,SLOT(on_btnEdit_clicked(i)));
        // emit on_btnEdit_clicked(i);

        //c++ 11 Lambda to call  on_btnEdit_clicked() function with index parameter to identify tableWidget row
        connect(button, &QPushButton::clicked, [this, button](){
            on_deleteBtn_clicked(button->property("btnDeleteId").toInt());
        });
        //Increases currentRowF
        currentRow = currentRow + 1;
    }

    delete db;
}

void MainWindow::on_groupsBtn_clicked()
{
    MainWindow::load_groups();

    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_newGroupBtn_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}


void MainWindow::on_searchGameExeBtn_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Get App EXE");
    QDir d = QFileInfo(filePath).absoluteDir();
    QString absolute = d.absoluteFilePath(filePath);

    // data = absolute;
    data.append(absolute);

    int currentRow = 0;
    ui->tableWidget_2->setRowCount(data.count());
    for (int i = 0; i < data.count(); ++i) {
        QFileInfo fileInfo(data[i]);
        QString fileName = fileInfo.fileName();  // Get the file name

        fileName.replace(".exe","");
        ui->tableWidget_2->setColumnWidth(0, 220);
        ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(fileName));


        // CELL BUTTON

        QPushButton* button = new QPushButton();
        button->setText("X");
        button->setStyleSheet("QPushButton {    background-color: rgb(41, 98, 255);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;	border-radius: 20px;	border-style: outset;}QPushButton::hover{     background-color: rgb(33, 78, 203);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}QPushButton::focus:pressed{ 	background-color: rgb(38, 72, 184);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}");
        //Sets button property to identify button
        button->setProperty("btnDeleteAppExeId", i);
        //Adds button to current index row
        ui->tableWidget_2->setCellWidget(currentRow, 1,  button);
        // Set the column width to fit the widget (e.g., 100 pixels)
        ui->tableWidget_2->setColumnWidth(1, 30);
        //emit connect(button,SIGNAL(clicked()),this,SLOT(on_btnEdit_clicked(i)));
        // emit on_btnEdit_clicked(i);

        //c++ 11 Lambda to call  on_btnEdit_clicked() function with index parameter to identify tableWidget row
        connect(button, &QPushButton::clicked, [this, button](){
           int itemToDeleteIndex = button->property("btnDeleteAppExeId").toInt();
            MainWindow::on_btn_push_back_data(itemToDeleteIndex);
        });
        //Increases currentRowF
        currentRow = currentRow + 1;
    }
}

void MainWindow::on_btn_push_back_data(int btnId){
    data.remove(btnId);

    int currentRow = 0;
    ui->tableWidget_2->setRowCount(data.count());
    for (int i = 0; i < data.count(); ++i) {
        QFileInfo fileInfo(data[i]);
        QString fileName = fileInfo.fileName();  // Get the file name

        fileName.replace(".exe","");
        ui->tableWidget_2->setColumnWidth(0, 220);
        ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(fileName));


        // CELL BUTTON

        QPushButton* button = new QPushButton();
        button->setText("X");
        button->setStyleSheet("QPushButton {    background-color: rgb(41, 98, 255);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;	border-radius: 20px;	border-style: outset;}QPushButton::hover{     background-color: rgb(33, 78, 203);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}QPushButton::focus:pressed{ 	background-color: rgb(38, 72, 184);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}");
        //Sets button property to identify button
        button->setProperty("btnDeleteAppExeId", i);
        //Adds button to current index row
        ui->tableWidget_2->setCellWidget(currentRow, 1,  button);
        // Set the column width to fit the widget (e.g., 100 pixels)
        ui->tableWidget_2->setColumnWidth(1, 30);
        //emit connect(button,SIGNAL(clicked()),this,SLOT(on_btnEdit_clicked(i)));
        // emit on_btnEdit_clicked(i);

        //c++ 11 Lambda to call  on_btnEdit_clicked() function with index parameter to identify tableWidget row
        connect(button, &QPushButton::clicked, [this, button](){
            int itemToDeleteIndex = button->property("btnDeleteAppExeId").toInt();
            MainWindow::on_btn_push_back_data(itemToDeleteIndex);
        });
        //Increases currentRowF
        currentRow = currentRow + 1;
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

void MainWindow::on_deleteBtn_clicked(int groupId)
{
    DbManager *db = new DbManager(path);

    bool deleteResult = db->deleteGroup(groupId);
    if(deleteResult){
        MainWindow::load_groups();
    }else{
        qDebug() << "Error";
    }

}


void MainWindow::on_groupBtn_clicked()
{
    groups* gp = new groups(nullptr, "apps");
    gp->show();
}

