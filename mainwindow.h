#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    const QString path = "tgroups.db";
    QVector<QString> data;

private slots:
    void on_newGroupBtn_clicked();

    void on_groupsBtn_clicked();

    void on_searchGameExeBtn_clicked();

    void on_saveBtn_clicked();

    void on_groupBtn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
