#include "groups.h"
#include "qdir.h"
#include "ui_groups.h"
#include "dbmanager.h"
#include <QCursor>
#include <QGridLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QEvent>
#include <QProcess>
#include <windows.h>
#include <QPixmap>
#include <Windows.h>
#include <QPainter>
#include "util.h"
#include <QPainterPath>
#include <QPropertyAnimation>

groups::groups(QWidget *parent, QString groupName)
    : QWidget(parent)
    , ui(new Ui::groups)
    ,m_groupName(groupName)
{
    ui->setupUi(this);

    // Set the window to have a transparent background
    //setAttribute(Qt::WA_TranslucentBackground);

    // Remove window frame if desired (optional)
    setWindowFlags(Qt::FramelessWindowHint);

    // Set the window opacity to be semi-transparent (optional)
   setWindowOpacity(0.9);

    setWindowFlags(windowFlags() | Qt::Popup);
   //this->setFocusPolicy(Qt::StrongFocus);  // Make the window focusable
  //  setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint); // Optionally keep window always on top, if needed
  //this->setWindowState(Qt::WindowActive);
    // Set the window to stay on top of other windows

    // Install event filter to track mouse events globally
    QApplication::instance()->installEventFilter(this);

    // Set the window to stay on top of other windows
    //setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    // Create a central widget (if not already done in the .ui file)
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setStyleSheet("background-color: transparent;");

    //setCentralWidget(centralWidget);

    // Create a grid layout
    QGridLayout* layout = new QGridLayout;


    QString appDir = QCoreApplication::applicationDirPath();
    QString dbPath = QDir(appDir).filePath("tgroups.db");
    qDebug() << dbPath;
    DbManager *db = new DbManager(dbPath);

    QVector<DbManager::Group> groups;
    groups = db->getOneGroup(m_groupName);


    QVector<QString> groups_app_patch;

    for (int var = 0; var < groups.count(); ++var) {
        // Split the string by commas and add each element to the vector
        groups_app_patch = groups[var].data.split(",");  // Split by comma

        // Trim spaces from each element (but keep quotes if present)
        for (int i = 0; i < groups_app_patch.size(); ++i) {
            // Trim only the spaces, leaving the quotes intact
            groups_app_patch[i] = groups_app_patch[i].trimmed();
        }
    }

    // Create and add buttons to the layout
    int row = 0, col = 0;
    int maxCols = 8;  // Maximum number of buttons in one row
    int buttonWidth = 28;
    int buttonHeight = 28;


    for (int i = 0; i < groups_app_patch.count(); ++i) {
        QFileInfo fileInfo(groups_app_patch[i]);
        QString fileName = fileInfo.fileName();  // Get the file name

        fileName.replace(".exe","");  // Clean the name, remove extension
        QChar lastChar = fileName.right(1).at(0).toUpper();  // Get last character of the name
        // Get the last character of the file name

        //QString(fileName)[0].toUpper() + QString(lastChar)
        // Create the button
        QPushButton* button = new QPushButton();
        qDebug() << "FILENAME " << lastChar;

        // Set button style (optional)
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: transparent;" // Transparent background
            "    font: 900 9pt 'Arial Black';"  // Font style
            "    color: rgb(255, 255, 255);"    // Text color
            "    border: none;"                  // No border
             "    border-radius: 6px;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(128, 128, 128, 0.5);" // Semi-transparent gray background on hover
            "    color: rgb(255, 255, 255);"                  // Text color on hover
            "    border: none;"                               // No border on hover
            "    border-radius: 6px;"                       // Rounded corners (adjust the radius as needed)
            "}"
            );

        // Set button size
        button->setFixedSize(28, 28);

        // Extract the icon directly into a QPixmap
        // Extract the icon directly into a QPixmap
        Util* util = new Util();
        QPixmap pixmap = util->extractPixmapFromExe(groups_app_patch[i]);

        bool isBlack = util->isPixmapBlack(pixmap);
        qDebug() << "isBlack " << isBlack;
        // Check if the pixmap is valid
        // If pixmap isBlack = true, adds text instead of icon to button
        if (pixmap.isNull() || isBlack) {
            button->setText(QString(fileName)[0].toUpper() + QString(lastChar));
            qDebug() << "Failed to extract icon from:" << groups_app_patch[i];
        } else {
            qDebug() << "Icon extracted successfully for:" << groups_app_patch[i];
            qDebug() << "Pixmap size:" << pixmap.size();

            // Save the pixmap to a file for debugging
            if (!pixmap.save("debug_icon.png")) {
                qDebug() << "Failed to save extracted icon to file!";
            } else {
                qDebug() << "Icon saved to debug_icon.png" << groups_app_patch[i];
            }

            // Create a QIcon from the QPixmap
            QIcon icon(pixmap);
            if (icon.isNull()) {
                qDebug() << "Failed to create QIcon from QPixmap!";
            } else {
                qDebug() << "QIcon created successfully.";
            }

            // Set the icon and icon size on the button
            button->setIcon(icon);
            button->setIconSize(QSize(24, 24)); // Ensure this matches the expected size
            qDebug() << "Button icon size:" << button->iconSize();

            // Force a repaint of the button
            button->repaint();

            // Debug button properties
            qDebug() << "Button size:" << button->size();
            qDebug() << "Button is visible:" << button->isVisible();
        }


        //label->show();
        // Add the button to the layout
        layout->addWidget(button, row, col);

        // Connect the button to launch the application when clicked
        connect(button, &QPushButton::clicked, this, [i, groups_app_patch]() {
            QString appPath = groups_app_patch[i];
            /*if (appPath.contains(" ")) {
                appPath = "\"" + appPath + "\"";  // Wrap in quotes if path contains spaces
            }*/
            // Create a QProcess instance
            QProcess *process = new QProcess();  // Note: Pass 'this' as the parent to avoid memory leaks

            // Get the directory of the executable
            QFileInfo fileInfo(appPath);
            QString workingDirectory = fileInfo.absolutePath(); // Get the directory where the exe is located

            // Set the working directory of the process to the location of the exe
            process->setWorkingDirectory(workingDirectory);

            // Start the process (instead of startDetached())
            process->start(appPath);

            // Check if the process started successfully
            if (!process->waitForStarted()) {
                qDebug() << "Failed to start the process.";
            }
        });

        // Update layout position
        col++;
        if (col == maxCols) {
            col = 0;
            row++;
        }
    }

    // Set the layout to the central widget
    centralWidget->setLayout(layout);
    int totalButtons = groups_app_patch.count();

    // Dynamically calculate number of columns (max 8 columns, or use all buttons if fewer than maxCols)
    int totalCols = std::min(maxCols, totalButtons);  // Ensure columns don't exceed maxCols
    int totalRows = (totalButtons / totalCols) + (totalButtons % totalCols == 0 ? 0 : 1);  // Calculate rows needed

    // Calculate window width and height
    int WWidth = buttonWidth * totalCols;  // Width is based on the number of columns (maxCols)
    int WHeight = buttonHeight * totalRows;  // Height depends on the number of rows required

    // temporal window width fix
    switch (totalCols) {
    case 1:
        WWidth = WWidth + 18;
        WHeight = WHeight + 20;
        break;
    case 2:
        WWidth = WWidth + 23;
        WHeight = WHeight + 20;
        break;
    case 3:
        WWidth = WWidth + 30;
        WHeight = WHeight + 20;
        break;
    case 4:
        WWidth = WWidth + 35;
        WHeight = WHeight + 20;
        break;
    case 5:
        WWidth = WWidth + 40;
        WHeight = WHeight + 20;
        break;
    case 6:
        WWidth = WWidth + 45;
        WHeight = WHeight + 20;
        break;
    case 7:
        WWidth = WWidth + 50;
        WHeight = WHeight + 20;
        break;
    case 8:
        WWidth = WWidth + 54;
        WHeight = WHeight + 20;
        break;
    default:
        break;
    }
    resize(WWidth, WHeight);

    // Create a rounded rectangle shape for the window
    QRegion region(0, 0, width(), height(), QRegion::Rectangle);  // Start with a normal rectangle
    QPainterPath path;
    path.addRoundedRect(0, 0, width(), height(), 5, 5);  // 20 is the radius for rounded corners
    region = QRegion(path.toFillPolygon().toPolygon());

    // Apply the region to the window to clip it
    setMask(region);

    // Get the current position of the mouse
    QPoint globalMousePos = QCursor::pos();

    // Get the screen's available geometry (excluding taskbar)
    QRect availableGeometry = QGuiApplication::primaryScreen()->availableGeometry();

    // Get the width and height of the window
    int windowWidth = this->width();
    int windowHeight = this->height();

    // Calculate the position for the window
    int xPos = globalMousePos.x() - windowWidth / 2;
    int yPos = availableGeometry.bottom() - windowHeight; // Just above the taskbar

    // Move the window to the calculated position
    move(xPos, yPos + 140);

    // Start the animation to move the window to the target position
    QPropertyAnimation *animation = new QPropertyAnimation(this, "pos");
    animation->setDuration(200);  // Set the duration of the animation (500 ms)
    animation->setStartValue(this->pos());  // Start at the current position
    animation->setEndValue(QPoint(xPos, yPos));  // Target position
    animation->setEasingCurve(QEasingCurve::OutQuad);  // Smooth easing curve for the animation
    animation->start(QAbstractAnimation::DeleteWhenStopped);  // Start the animation and delete when finished
}

groups::~groups()
{
    delete ui;
}


