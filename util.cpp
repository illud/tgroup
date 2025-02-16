#include "util.h"
#include "qfileinfo.h"
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

Util::Util() {}

QPixmap Util::extractPixmapFromExe(const QString &exePath) {
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

bool Util::isPixmapBlack(const QPixmap& pixmap) {
    // Convert QPixmap to QImage
    QImage image = pixmap.toImage();

    // Check if the image is empty
    if (image.isNull()) {
        qDebug() << "The image is empty.";
        return false;
    }

    // Iterate through the pixels of the QImage
    bool isBlack = true;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor color = image.pixelColor(x, y);
            // Check if the pixel is not black (i.e., red, green, blue are not 0)
            if (color.alpha() > 0 && (color.red() != 0 || color.green() != 0 || color.blue() != 0)) {
                isBlack = false;
                break;
            }
        }
        if (!isBlack) break;
    }

    return isBlack;
}
