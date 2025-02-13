#ifndef GROUP_H
#define GROUP_H

#include <QMainWindow>

namespace Ui {
class Group;
}

class Group : public QMainWindow
{
    Q_OBJECT

public:
    explicit Group(QWidget *parent = nullptr);
    ~Group();

private:
    Ui::Group *ui;

protected:
    void focusOutEvent(QFocusEvent *event) override; // Reimplement focusOutEvent
};

#endif // GROUP_H
