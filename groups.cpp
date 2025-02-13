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
        button->setStyleSheet(
            "QPushButton {"
            "    background-color: transparent;" // Transparent background
            "    font: 900 9pt 'Arial Black';"  // Font style
            "    color: rgb(255, 255, 255);"    // Text color
            "    border: none;"                  // No border
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(128, 128, 128, 0.5);" // Semi-transparent gray background on hover
            "    color: rgb(255, 255, 255);"                  // Text color on hover
            "    border: none;"                               // No border on hover
            "    border-radius: 15px;"                       // Rounded corners (adjust the radius as needed)
            "}"
            );

        // Set button size
        button->setFixedSize(28, 28);

        // Extract the icon directly into a QPixmap
        // Extract the icon directly into a QPixmap
        QPixmap pixmap = extractPixmapFromExe(groups_app_patch[i]);

        // Check if the pixmap is valid
        if (pixmap.isNull()) {
            qDebug() << "Failed to extract icon from:" << groups_app_patch[i];
        } else {
            qDebug() << "Icon extracted successfully for:" << groups_app_patch[i];
            qDebug() << "Pixmap size:" << pixmap.size();

            // Save the pixmap to a file for debugging
            if (!pixmap.save("debug_icon.png")) {
                qDebug() << "Failed to save extracted icon to file!";
            } else {
                qDebug() << "Icon saved to debug_icon.png";
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
            if (appPath.contains(" ")) {
                appPath = "\"" + appPath + "\"";  // Wrap in quotes if path contains spaces
            }
            QProcess::startDetached(appPath);  // Launch the .exe
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

    // Extract the icon from the executable
    HICON hIcon = ExtractIcon(NULL, exePath.toStdWString().c_str(), 0);
    if (hIcon == NULL) {
        qDebug() << "Failed to extract icon from" << exePath;
        return QPixmap(); // Return an empty pixmap if icon extraction fails
    }

    // Get icon information
    ICONINFO iconInfo;
    if (!GetIconInfo(hIcon, &iconInfo)) {
        qDebug() << "Failed to get icon info.";
        DestroyIcon(hIcon);
        return QPixmap();
    }

    // Get the color bitmap information
    BITMAP bitmap;
    if (!GetObject(iconInfo.hbmColor, sizeof(bitmap), &bitmap)) {
        qDebug() << "Failed to get bitmap info.";
        DestroyIcon(hIcon);
        return QPixmap();
    }

    // Create a device context for the bitmap
    HDC hdc = GetDC(NULL);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, iconInfo.hbmColor);

    // Prepare the BITMAPINFO structure
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bitmap.bmWidth;
    bmi.bmiHeader.biHeight = -bitmap.bmHeight; // Negative for top-down bitmap
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Allocate a buffer for the bitmap data
    QByteArray buffer(bitmap.bmWidth * bitmap.bmHeight * 4, 0); // 32-bit ARGB format

    // Retrieve the bitmap data
    if (!GetDIBits(memDC, iconInfo.hbmColor, 0, bitmap.bmHeight, buffer.data(), &bmi, DIB_RGB_COLORS)) {
        qDebug() << "Failed to retrieve bitmap data.";
        SelectObject(memDC, hOldBitmap);
        DeleteDC(memDC);
        ReleaseDC(NULL, hdc);
        DestroyIcon(hIcon);
        return QPixmap();
    }

    // Create a QImage from the bitmap data
    QImage img(reinterpret_cast<const uchar*>(buffer.constData()), bitmap.bmWidth, bitmap.bmHeight, QImage::Format_ARGB32);

    // Clean up resources
    SelectObject(memDC, hOldBitmap);
    DeleteDC(memDC);
    ReleaseDC(NULL, hdc);
    DestroyIcon(hIcon);

    // Convert the QImage to a QPixmap and return it
    return QPixmap::fromImage(img);
}
