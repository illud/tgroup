#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "groups.h"
#include "dbmanager.h"
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QCoreApplication>
#include "util.h"
#include <QStatusBar>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // tableWidgets
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_2->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_3->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableWidget_3->setEditTriggers(QAbstractItemView::NoEditTriggers);

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
        ui->tableWidget->setColumnWidth(0, 145);
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(groups[i].groupName));

        // Create the "Edit" button
        QPushButton* editButton = new QPushButton();
        editButton->setText("Edit");  // Set the text for the Edit button
        editButton->setStyleSheet("QPushButton {"
                                  "background-color: rgb(38, 72, 184);"
                                  "font: 900 9pt 'Arial Black';"
                                  "border-radius: 10px;"
                                  "color: rgb(255, 255, 255);"
                                  "border: 0px;"
                                  "border-radius: 20px;"
                                  "border-style: outset;"
                                  "}QPushButton::hover{"
                                  "background-color: rgb(38, 72, 184);"
                                  "font: 900 9pt 'Arial Black';"
                                  "color: rgb(255, 255, 255);"
                                  "border: 0px;"
                                  "}QPushButton::focus:pressed{"
                                  "background-color: rgb(38, 72, 184);"
                                  "font: 900 9pt 'Arial Black';"
                                  "color: rgb(255, 255, 255);"
                                  "border: 0px;"
                                  "}");

        // Set button property to identify it (Edit)
        editButton->setProperty("btnEditId", groups[i].id);

        // Add the edit button to the current row and column (next to delete button)
        ui->tableWidget->setCellWidget(currentRow, 1, editButton);  // Column 2 for edit button

        // Set the column width for the "Edit" button
        ui->tableWidget->setColumnWidth(1, 60);  // Adjust the width for the edit button

        // Connect the "Edit" button to its handler (using lambda)
        connect(editButton, &QPushButton::clicked, [this, editButton](){
            on_editBtn_clicked(editButton->property("btnEditId").toInt());
        });

        // Create the "Delete" button
        QPushButton* deleteButton = new QPushButton();
        deleteButton->setText("X");
        deleteButton->setStyleSheet("QPushButton {"
                                    "background-color: rgb(255, 41, 77);"
                                    "font: 900 9pt 'Arial Black';"
                                    "border-radius: 10px;"
                                    "color: rgb(255, 255, 255);"
                                    "border: 0px;"
                                    "border-radius: 20px;"
                                    "border-style: outset;"
                                    "}QPushButton::hover{"
                                    "background-color: rgb(255, 41, 41);"
                                    "font: 900 9pt 'Arial Black';"
                                    "color: rgb(255, 255, 255);"
                                    "border: 0px;"
                                    "}QPushButton::focus:pressed{"
                                    "background-color: rgb(38, 72, 184);"
                                    "font: 900 9pt 'Arial Black';"
                                    "color: rgb(255, 255, 255);"
                                    "border: 0px;"
                                    "}");

        // Set button property to identify it (Delete)
        deleteButton->setProperty("btnDeleteId", groups[i].id);

        // Add the delete button to the current row and column
        ui->tableWidget->setCellWidget(currentRow, 2, deleteButton);

        // Set the column width to fit the delete button
        ui->tableWidget->setColumnWidth(2, 30);

        // Connect delete button to its handler (using lambda)
        connect(deleteButton, &QPushButton::clicked, [this, deleteButton](){
            on_deleteBtn_clicked(deleteButton->property("btnDeleteId").toInt());
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
    data.clear();
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
        QChar lastChar = fileName.right(1).at(0).toUpper();  // Get last character of the name
        fileName.replace(".exe","");

        Util* util = new Util();
        QPixmap pixmap = util->extractPixmapFromExe(data[i]);


        // Check if the pixmap is valid
        bool isBlack = util->isPixmapBlack(pixmap);
        // If pixmap isBlack = true, adds text instead of icon to button
        if (pixmap.isNull() || isBlack) {
            ui->tableWidget_2->setColumnWidth(0, 220);
            ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(QString(fileName)[0].toUpper() + QString(lastChar)));

        } else {
            // Save the pixmap to a file for debugging
            if (!pixmap.save("debug_icon.png")) {
                qDebug() << "Failed to save extracted icon to file!";
            } else {
                qDebug() << "Icon saved to debug_icon.png" << data[i];
            }

            // Create a QIcon from the QPixmap
            QIcon icon(pixmap);
            if (icon.isNull()) {
                qDebug() << "Failed to create QIcon from QPixmap!";
            } else {
                qDebug() << "QIcon created successfully.";
            }


            // Create a new QTableWidgetItem
            QTableWidgetItem *item = new QTableWidgetItem();

            // Set the icon for the item (instead of a button)
            item->setIcon(icon);

            // Set the size of the icon, if necessary
            item->setData(Qt::DecorationRole, icon);

            // Optionally, set text or other properties
            item->setText(fileName); // If you want some text alongside the icon

            // Set the item to the specific row and column
            ui->tableWidget_2->setItem(i, 0, item);

            // Set the column width (this part remains unchanged)
            ui->tableWidget_2->setColumnWidth(0, 220);
        }




        // CELL BUTTON

        QPushButton* button = new QPushButton();
        button->setText("X");
        button->setStyleSheet("QPushButton {    background-color: rgb(255, 41, 77);	font: 900 9pt 'Arial Black';border-radius: 10px;	color: rgb(255, 255, 255);    border: 0px;	border-radius: 20px;	border-style: outset;}QPushButton::hover{     background-color: rgb(255, 41, 41);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}QPushButton::focus:pressed{ 	background-color: rgb(38, 72, 184);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}");
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
        QChar lastChar = fileName.right(1).at(0).toUpper();  // Get last character of the name
        fileName.replace(".exe","");

        Util* util = new Util();
        QPixmap pixmap = util->extractPixmapFromExe(data[i]);


        // Check if the pixmap is valid
        bool isBlack = util->isPixmapBlack(pixmap);
        // If pixmap isBlack = true, adds text instead of icon to button
        if (pixmap.isNull() || isBlack) {
            ui->tableWidget_2->setColumnWidth(0, 220);
            ui->tableWidget_2->setItem(i,0,new QTableWidgetItem(QString(fileName)[0].toUpper() + QString(lastChar)));

        } else {
            // Save the pixmap to a file for debugging
            if (!pixmap.save("debug_icon.png")) {
                qDebug() << "Failed to save extracted icon to file!";
            } else {
                qDebug() << "Icon saved to debug_icon.png" << data[i];
            }

            // Create a QIcon from the QPixmap
            QIcon icon(pixmap);
            if (icon.isNull()) {
                qDebug() << "Failed to create QIcon from QPixmap!";
            } else {
                qDebug() << "QIcon created successfully.";
            }


            // Create a new QTableWidgetItem
            QTableWidgetItem *item = new QTableWidgetItem();

            // Set the icon for the item (instead of a button)
            item->setIcon(icon);

            // Set the size of the icon, if necessary
            item->setData(Qt::DecorationRole, icon);

            // Optionally, set text or other properties
            item->setText(fileName); // If you want some text alongside the icon

            // Set the item to the specific row and column
            ui->tableWidget_2->setItem(i, 0, item);

            // Set the column width (this part remains unchanged)
            ui->tableWidget_2->setColumnWidth(0, 220);
        }
        // CELL BUTTON

        QPushButton* button = new QPushButton();
        button->setText("X");
        button->setStyleSheet("QPushButton {    background-color: rgb(255, 41, 77);	font: 900 9pt 'Arial Black';border-radius: 10px;	color: rgb(255, 255, 255);    border: 0px;	border-radius: 20px;	border-style: outset;}QPushButton::hover{     background-color: rgb(255, 41, 41);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}QPushButton::focus:pressed{ 	background-color: rgb(38, 72, 184);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}");
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
        // Set the status bar style
        statusBar()->setStyleSheet("color: #ffffff; background-color: #388E3C; font-size: 13px;");
        statusBar()->show();
        // Create a QTimer to hide the message after 3 seconds
        QTimer::singleShot(3000, statusBar(), [this]()
                           {
                               statusBar()->clearMessage(); // Clear the message after 3 seconds
                               statusBar()->hide();         // Hide the status bar
                           });

        // Show the status message
        statusBar()->showMessage(tr("Shortcut created successfully!"));

        data.clear();
        MainWindow::load_groups();
        ui->stackedWidget->setCurrentIndex(0);

    } else {
        // Set the status bar style
        statusBar()->setStyleSheet("color: #ffffff; background-color: ##8e3838; font-size: 13px;");
        statusBar()->show();
        // Create a QTimer to hide the message after 3 seconds
        QTimer::singleShot(3000, statusBar(), [this]()
                           {
                               statusBar()->clearMessage(); // Clear the message after 3 seconds
                               statusBar()->hide();         // Hide the status bar
                           });

        // Show the status message
        statusBar()->showMessage(tr("Failed to create the shortcut!"));
    }

    delete db;
}

void MainWindow::on_deleteBtn_clicked(int groupId)
{
    DbManager *db = new DbManager(path);

    bool deleteResult = db->deleteGroup(groupId);
    if(deleteResult){
        // Set the status bar style
        statusBar()->setStyleSheet("color: #ffffff; background-color: #388E3C; font-size: 13px;");
        statusBar()->show();
        // Create a QTimer to hide the message after 3 seconds
        QTimer::singleShot(3000, statusBar(), [this]()
                           {
                               statusBar()->clearMessage(); // Clear the message after 3 seconds
                               statusBar()->hide();         // Hide the status bar
                           });

        // Show the status message
        statusBar()->showMessage(tr("Group deleted successfully!"));
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

void MainWindow::on_editBtn_clicked(int groupId){
    data.clear();

    DbManager *db = new DbManager(path);

    QVector<DbManager::Group> groups;
    groups = db->getOneGroupById(groupId);

    editGroupId = groups[0].id;
    oldGroupName = groups[0].groupName;

    ui->lineEdit_2->setText(groups[0].groupName);


    QVector<QString> apps = groups[0].data.split(",");

    for (int i = 0; i < apps.count(); ++i) {
        data.append(apps[i].trimmed());
    }

    int currentRow = 0;
    ui->tableWidget_3->setRowCount(data.count());
    for (int i = 0; i < data.count(); ++i) {
        QFileInfo fileInfo(data[i]);
        QString fileName = fileInfo.fileName();  // Get the file name
        QChar lastChar = fileName.right(1).at(0).toUpper();  // Get last character of the name
        fileName.replace(".exe","");

        Util* util = new Util();
        QPixmap pixmap = util->extractPixmapFromExe(data[i]);


        // Check if the pixmap is valid
        bool isBlack = util->isPixmapBlack(pixmap);
        // If pixmap isBlack = true, adds text instead of icon to button
        if (pixmap.isNull() || isBlack) {
            ui->tableWidget_3->setColumnWidth(0, 220);
            ui->tableWidget_3->setItem(i,0,new QTableWidgetItem(QString(fileName)[0].toUpper() + QString(lastChar)));

        } else {
            // Save the pixmap to a file for debugging
            if (!pixmap.save("debug_icon.png")) {
                qDebug() << "Failed to save extracted icon to file!";
            } else {
                qDebug() << "Icon saved to debug_icon.png" << data[i];
            }

            // Create a QIcon from the QPixmap
            QIcon icon(pixmap);
            if (icon.isNull()) {
                qDebug() << "Failed to create QIcon from QPixmap!";
            } else {
                qDebug() << "QIcon created successfully.";
            }


            // Create a new QTableWidgetItem
            QTableWidgetItem *item = new QTableWidgetItem();

            // Set the icon for the item (instead of a button)
            item->setIcon(icon);

            // Set the size of the icon, if necessary
            item->setData(Qt::DecorationRole, icon);

            // Optionally, set text or other properties
            item->setText(fileName); // If you want some text alongside the icon

            // Set the item to the specific row and column
            ui->tableWidget_3->setItem(i, 0, item);

            // Set the column width (this part remains unchanged)
            ui->tableWidget_3->setColumnWidth(0, 220);
        }




        // CELL BUTTON

        QPushButton* button = new QPushButton();
        button->setText("X");
        button->setStyleSheet("QPushButton {    background-color: rgb(255, 41, 77);	font: 900 9pt 'Arial Black';border-radius: 10px;	color: rgb(255, 255, 255);    border: 0px;	border-radius: 20px;	border-style: outset;}QPushButton::hover{     background-color: rgb(255, 41, 41);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}QPushButton::focus:pressed{ 	background-color: rgb(38, 72, 184);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}");
        //Sets button property to identify button
        button->setProperty("btnDeleteAppExeId", i);
        //Adds button to current index row
        ui->tableWidget_3->setCellWidget(currentRow, 1,  button);
        // Set the column width to fit the widget (e.g., 100 pixels)
        ui->tableWidget_3->setColumnWidth(1, 30);
        //emit connect(button,SIGNAL(clicked()),this,SLOT(on_btnEdit_clicked(i)));
        // emit on_btnEdit_clicked(i);

        //c++ 11 Lambda to call  on_btnEdit_clicked() function with index parameter to identify tableWidget row
        connect(button, &QPushButton::clicked, [this, button](){
            int itemToDeleteIndex = button->property("btnDeleteAppExeId").toInt();
            MainWindow::on_btn_push_back_edit_data(itemToDeleteIndex);
        });
        //Increases currentRowF
        currentRow = currentRow + 1;
    }

    ui->tableWidget_3->repaint();
    delete db;

    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_searchGameExeBtn_2_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Get App EXE");
    QDir d = QFileInfo(filePath).absoluteDir();
    QString absolute = d.absoluteFilePath(filePath);

    // data = absolute;
    data.append(absolute);

    int currentRow = 0;
    ui->tableWidget_3->setRowCount(data.count());
    for (int i = 0; i < data.count(); ++i) {
        QFileInfo fileInfo(data[i]);
        QString fileName = fileInfo.fileName();  // Get the file name
        QChar lastChar = fileName.right(1).at(0).toUpper();  // Get last character of the name
        fileName.replace(".exe","");

        Util* util = new Util();
        QPixmap pixmap = util->extractPixmapFromExe(data[i]);


        // Check if the pixmap is valid
        bool isBlack = util->isPixmapBlack(pixmap);
        // If pixmap isBlack = true, adds text instead of icon to button
        if (pixmap.isNull() || isBlack) {
            ui->tableWidget_3->setColumnWidth(0, 220);
            ui->tableWidget_3->setItem(i,0,new QTableWidgetItem(QString(fileName)[0].toUpper() + QString(lastChar)));

        } else {
            // Save the pixmap to a file for debugging
            if (!pixmap.save("debug_icon.png")) {
                qDebug() << "Failed to save extracted icon to file!";
            } else {
                qDebug() << "Icon saved to debug_icon.png" << data[i];
            }

            // Create a QIcon from the QPixmap
            QIcon icon(pixmap);
            if (icon.isNull()) {
                qDebug() << "Failed to create QIcon from QPixmap!";
            } else {
                qDebug() << "QIcon created successfully.";
            }


            // Create a new QTableWidgetItem
            QTableWidgetItem *item = new QTableWidgetItem();

            // Set the icon for the item (instead of a button)
            item->setIcon(icon);

            // Set the size of the icon, if necessary
            item->setData(Qt::DecorationRole, icon);

            // Optionally, set text or other properties
            item->setText(fileName); // If you want some text alongside the icon

            // Set the item to the specific row and column
            ui->tableWidget_3->setItem(i, 0, item);

            // Set the column width (this part remains unchanged)
            ui->tableWidget_3->setColumnWidth(0, 220);
        }




        // CELL BUTTON

        QPushButton* button = new QPushButton();
        button->setText("X");
        button->setStyleSheet("QPushButton {    background-color: rgb(255, 41, 77);	font: 900 9pt 'Arial Black';border-radius: 10px;	color: rgb(255, 255, 255);    border: 0px;	border-radius: 20px;	border-style: outset;}QPushButton::hover{     background-color: rgb(255, 41, 41);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}QPushButton::focus:pressed{ 	background-color: rgb(38, 72, 184);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}");
        //Sets button property to identify button
        button->setProperty("btnDeleteAppExeId", i);
        //Adds button to current index row
        ui->tableWidget_3->setCellWidget(currentRow, 1,  button);
        // Set the column width to fit the widget (e.g., 100 pixels)
        ui->tableWidget_3->setColumnWidth(1, 30);
        //emit connect(button,SIGNAL(clicked()),this,SLOT(on_btnEdit_clicked(i)));
        // emit on_btnEdit_clicked(i);

        //c++ 11 Lambda to call  on_btnEdit_clicked() function with index parameter to identify tableWidget row
        connect(button, &QPushButton::clicked, [this, button](){
            int itemToDeleteIndex = button->property("btnDeleteAppExeId").toInt();
            MainWindow::on_btn_push_back_edit_data(itemToDeleteIndex);
        });
        //Increases currentRowF
        currentRow = currentRow + 1;
    }

        ui->tableWidget_3->repaint();
}

void MainWindow::on_btn_push_back_edit_data(int btnId){
    data.remove(btnId);

    int currentRow = 0;
    ui->tableWidget_3->setRowCount(data.count());
    for (int i = 0; i < data.count(); ++i) {
        QFileInfo fileInfo(data[i]);
        QString fileName = fileInfo.fileName();  // Get the file name
        QChar lastChar = fileName.right(1).at(0).toUpper();  // Get last character of the name
        fileName.replace(".exe","");

        Util* util = new Util();
        QPixmap pixmap = util->extractPixmapFromExe(data[i]);


        // Check if the pixmap is valid
        bool isBlack = util->isPixmapBlack(pixmap);
        // If pixmap isBlack = true, adds text instead of icon to button
        if (pixmap.isNull() || isBlack) {
            ui->tableWidget_3->setColumnWidth(0, 220);
            ui->tableWidget_3->setItem(i,0,new QTableWidgetItem(QString(fileName)[0].toUpper() + QString(lastChar)));

        } else {
            // Save the pixmap to a file for debugging
            if (!pixmap.save("debug_icon.png")) {
                qDebug() << "Failed to save extracted icon to file!";
            } else {
                qDebug() << "Icon saved to debug_icon.png" << data[i];
            }

            // Create a QIcon from the QPixmap
            QIcon icon(pixmap);
            if (icon.isNull()) {
                qDebug() << "Failed to create QIcon from QPixmap!";
            } else {
                qDebug() << "QIcon created successfully.";
            }


            // Create a new QTableWidgetItem
            QTableWidgetItem *item = new QTableWidgetItem();

            // Set the icon for the item (instead of a button)
            item->setIcon(icon);

            // Set the size of the icon, if necessary
            item->setData(Qt::DecorationRole, icon);

            // Optionally, set text or other properties
            item->setText(fileName); // If you want some text alongside the icon

            // Set the item to the specific row and column
            ui->tableWidget_3->setItem(i, 0, item);

            // Set the column width (this part remains unchanged)
            ui->tableWidget_3->setColumnWidth(0, 220);
        }
        // CELL BUTTON

        QPushButton* button = new QPushButton();
        button->setText("X");
        button->setStyleSheet("QPushButton {    background-color: rgb(255, 41, 77);	font: 900 9pt 'Arial Black';border-radius: 10px;	color: rgb(255, 255, 255);    border: 0px;	border-radius: 20px;	border-style: outset;}QPushButton::hover{     background-color: rgb(255, 41, 41);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}QPushButton::focus:pressed{ 	background-color: rgb(38, 72, 184);	font: 900 9pt 'Arial Black';	color: rgb(255, 255, 255);    border: 0px;}");
        //Sets button property to identify button
        button->setProperty("btnDeleteAppExeId", i);
        //Adds button to current index row
        ui->tableWidget_3->setCellWidget(currentRow, 1,  button);
        // Set the column width to fit the widget (e.g., 100 pixels)
        ui->tableWidget_3->setColumnWidth(1, 30);
        //emit connect(button,SIGNAL(clicked()),this,SLOT(on_btnEdit_clicked(i)));
        // emit on_btnEdit_clicked(i);

        //c++ 11 Lambda to call  on_btnEdit_clicked() function with index parameter to identify tableWidget row
        connect(button, &QPushButton::clicked, [this, button](){
            int itemToDeleteIndex = button->property("btnDeleteAppExeId").toInt();
            MainWindow::on_btn_push_back_edit_data(itemToDeleteIndex);
        });
        //Increases currentRowF
        currentRow = currentRow + 1;
    }
        ui->tableWidget_3->repaint();
}

void MainWindow::on_updateBtn_clicked()
{
    // Instance db conn
    DbManager *db = new DbManager(path);

    // Insert into games table
    QString result = QStringList::fromVector(data).join(", ");
    db->updateGroup(ui->lineEdit_2->text(), result, editGroupId);

    QString folderPath = "shortcuts";  // Folder where the shortcut will be created
    QString oldShortcutPath = folderPath + "/" + oldGroupName + ".lnk";

    QString shortcutPath = folderPath + "/" + ui->lineEdit_2->text() + ".lnk";

    // Check if shortcut already exists, if so, delete it
    QFile shortcutFile(oldShortcutPath);
    if (shortcutFile.exists()) {
        if (!shortcutFile.remove()) {
            qDebug() << "Failed to delete existing shortcut!";
            delete db;
            return; // Exit if the shortcut couldn't be deleted
        } else {
            qDebug() << "Existing shortcut deleted.";
        }
    }

    QString appDir = QCoreApplication::applicationDirPath();

    // Command to create a new shortcut using PowerShell script
    QString command = QString("powershell -Command \"$WshShell = New-Object -ComObject WScript.Shell; "
                              "$shortcut = $WshShell.CreateShortcut('%1'); "
                              "$shortcut.TargetPath = '%2\\tgroup.exe'; "
                              "$shortcut.Arguments = '%3'; "
                              "$shortcut.Save()\"").arg(shortcutPath).arg(appDir).arg(ui->lineEdit_2->text());

    QProcess process;
    process.start(command);
    process.waitForFinished();

    if (process.exitCode() == 0) {
        // Set the status bar style
        statusBar()->setStyleSheet("color: #ffffff; background-color: #388E3C; font-size: 13px;");
        statusBar()->show();
        // Create a QTimer to hide the message after 3 seconds
        QTimer::singleShot(3000, statusBar(), [this]()
                           {
                               statusBar()->clearMessage(); // Clear the message after 3 seconds
                               statusBar()->hide();         // Hide the status bar
                           });

        // Show the status message
        statusBar()->showMessage(tr("Shortcut updated successfully!"));

        data.clear();
        MainWindow::load_groups();
        ui->stackedWidget->setCurrentIndex(0);
    } else {
        qDebug() << "Failed to update the shortcut!";
        // Set the status bar style
        statusBar()->setStyleSheet("color: #ffffff; background-color: ##8e3838; font-size: 13px;");
        statusBar()->show();
        // Create a QTimer to hide the message after 3 seconds
        QTimer::singleShot(3000, statusBar(), [this]()
                           {
                               statusBar()->clearMessage(); // Clear the message after 3 seconds
                               statusBar()->hide();         // Hide the status bar
                           });

        // Show the status message
        statusBar()->showMessage(tr("Failed to update the shortcut!"));
    }

    delete db;
}


