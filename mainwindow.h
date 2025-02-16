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
    QVector<QString> dataEdit;
    int editGroupId;

private slots:
    void on_newGroupBtn_clicked();

    void on_groupsBtn_clicked();

    void on_searchGameExeBtn_clicked();

    void on_saveBtn_clicked();

    void on_groupBtn_clicked();

    void on_deleteBtn_clicked(int groupId);

    void load_groups();

    void on_btn_push_back_data(int btnId);

    void on_editBtn_clicked(int groupId);

    void on_searchGameExeBtn_2_clicked();
    void on_btn_push_back_edit_data(int btnId);

    void on_updateBtn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
