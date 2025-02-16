#ifndef UTILS_H
#define UTILS_H

#include <QObject>

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

public slots:
    QPixmap extractPixmapFromExe(const QString &exePath);
    bool isPixmapBlack(const QPixmap& pixmap) ;

signals:
};

#endif // UTILS_H
