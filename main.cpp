#include "mainwindow.h"
#include "groups.h"
#include "dbmanager.h"
#include "qdir.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString appDir = QCoreApplication::applicationDirPath();
    QString dbPath = QDir(appDir).filePath("tgroups.db");
    qDebug() << dbPath;
    DbManager *db = new DbManager(dbPath);

    QVector<DbManager::Group> groupsResult;
    groupsResult = db->getGroups();

    // Check if there are arguments
    if (argc > 1) {
        QString arg = QString(argv[1]);  // Safely access argv[1] if it exists
        for (int i = 0; i < groupsResult.count(); ++i) {
            // Handling the 'dev' argument
            if (arg == groupsResult[i].groupName) {
                groups gp(nullptr, groupsResult[i].groupName);
                gp.show();
                return a.exec();  // Event loop keeps the window open
            }
        }
    } else {
        // No argument passed, show the MainWindow by default
        MainWindow w;
        w.show();
        return a.exec();
    }

    return a.exec();  // Event loop keeps the window open
}
