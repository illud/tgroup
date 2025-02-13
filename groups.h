#ifndef GROUPS_H
#define GROUPS_H

#include <QWidget>

namespace Ui {
class groups;
}

class groups : public QWidget
{
    Q_OBJECT

public:
    explicit groups(QWidget *parent = nullptr, QString groupName = "");
    ~groups();
    const QString path = "tgroups.db";

private:
    Ui::groups *ui;
    QString m_groupName;

public slots:
    QPixmap extractPixmapFromExe(const QString &exePath);
protected:
};

#endif // GROUPS_H
