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


    // Get the current position of the mouse
    QPoint globalMousePos = QCursor::pos();

    // Calculate the position to center the window's top at the mouse position
    int windowWidth = this->width();
    int windowHeight = this->height();

    // Define the margin (top space) you want between the cursor and the window
    int marginTop = 50;

    // Move the window so that its top-center is at the mouse position, plus the margin
    int xPos = globalMousePos.x() - windowWidth / 2;
    int yPos = globalMousePos.y() - windowHeight - marginTop;  // Add margin to the vertical position

    move(xPos, yPos);

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
    int maxCols = 4;  // Maximum number of buttons in one row
    int buttonWidth = 32;
    int buttonHeight = 32;


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
        button->setStyleSheet("QPushButton {"
                              "background-color: transparent;"
                              "font: 900 9pt 'Arial Black';"
                              "color: rgb(255, 255, 255);"
                              "border: none;}"
                              "QPushButton:hover {"
                              "background-color: rgba(128, 128, 128, 0.5);"
                              "color: rgb(255, 255, 255);"
                              "border: none;}");

        // Set button size
        button->setFixedSize(32, 32);

        // Extract the icon directly into a QPixmap
        QPixmap pixmap = extractPixmapFromExe(groups_app_patch[i]);

        // Check if the pixmap is valid
        if (pixmap.isNull()) {
              qDebug() << "Failed to extract icon from:" << groups_app_patch[i];
        } else {
            // Set the pixmap as the button's icon
            button->setIcon(QIcon(pixmap));
            button->setIconSize(QSize(32, 32));  // Set the icon size to match the button

            qDebug() << "Icon extracted successfully for:" << groups_app_patch[i];
        }


        //label->show();
        // Add the button to the layout
        layout->addWidget(button, row, col);

        // Connect the button to launch the application when clicked
        connect(button, &QPushButton::clicked, this, [i, groups_app_patch]() {
            QString appPath = groups_app_patch[i];
            if (appPath.contains(" ")) {
                appPath = "\"" + appPath + "\"";  // Wrap in quotes if path contains spaces
            }
            QProcess::startDetached(appPath);  // Launch the .exe
        });
 layout->update();
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

    // Dynamically calculate number of columns (max 4 columns, or use all buttons if fewer than 4)
     // Limit to 4 columns, or use fewer based on totalButtons
    int totalRows = (totalButtons / maxCols) + (totalButtons % maxCols == 0 ? 0 : 1);  // Calculate number of rows needed

    // Calculate window width and height
    int width = buttonWidth * maxCols;  // Width for the number of columns
    int height = totalRows * buttonHeight;  // Height depends on the number of rows required

    // If there's only one button, resize the window to match the button size
    if (totalButtons == 1) {
        width = buttonWidth;  // Only one button, window should match button size
        height = buttonHeight;
    }

    // Set the window size based on calculated width and height
    resize(width + 34, height + 20);  // Add padding or margins as needed
}

groups::~groups()
{
    delete ui;
}

QPixmap groups::extractPixmapFromExe(const QString &exePath) {
    QFileInfo fileInfo(exePath);
    QString fileName = fileInfo.fileName();
    qDebug() << "Extracting icon from file: " << fileName;

    HICON hIcon = ExtractIcon(NULL, exePath.toStdWString().c_str(), 0);

    if (hIcon == NULL) {
        qDebug() << "Failed to extract icon from" << exePath;
        return QPixmap(); // Return an empty pixmap if icon extraction fails
    }

    // Extract icon information
    ICONINFO iconInfo;
    if (GetIconInfo(hIcon, &iconInfo)) {
        BITMAP bitmap;
        GetObject(iconInfo.hbmColor, sizeof(bitmap), &bitmap);

        HDC hdc = GetDC(NULL);
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP hBitmap = (HBITMAP)SelectObject(memDC, iconInfo.hbmColor);

        BITMAPINFO bitmapInfo = {};
        bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
        bitmapInfo.bmiHeader.biWidth = bitmap.bmWidth;
        bitmapInfo.bmiHeader.biHeight = -bitmap.bmHeight; // Negative to specify top-down bitmap
        bitmapInfo.bmiHeader.biPlanes = 1;
        bitmapInfo.bmiHeader.biBitCount = 32;

        int rowSize = ((bitmap.bmWidth * 32 + 31) / 32) * 4;
        QByteArray byteArray(rowSize * bitmap.bmHeight, 0);
        int bytesRead = GetDIBits(hdc, hBitmap, 0, bitmap.bmHeight, byteArray.data(), &bitmapInfo, DIB_RGB_COLORS);

        if (bytesRead == 0) {
            qDebug() << "Failed to retrieve icon bitmap data.";
            DestroyIcon(hIcon);
            return QPixmap();
        }

        // Create a QImage from the QByteArray buffer
        QImage img(reinterpret_cast<const uchar *>(byteArray.constData()), bitmap.bmWidth, bitmap.bmHeight, rowSize, QImage::Format_ARGB32);

        // Create and return a QPixmap from the QImage
        QPixmap pixmap = QPixmap::fromImage(img);
        DestroyIcon(hIcon); // Clean up the icon handle
        return pixmap;
    }

    DestroyIcon(hIcon); // Clean up the icon handle if it failed to get icon info
    return QPixmap();
}
