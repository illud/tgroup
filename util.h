#ifndef UTIL_H
#define UTIL_H

#include <QObject>

class Util : public QObject
{

    Q_OBJECT

public:
    Util();
    QPixmap extractPixmapFromExe(const QString &exePath);
    bool isPixmapBlack(const QPixmap& pixmap) ;
};

#endif // UTIL_H
