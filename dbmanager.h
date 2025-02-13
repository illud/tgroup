#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQueryModel>

class DbManager
{
private:
    QSqlDatabase m_db;

public:
    DbManager(const QString &path);
    /**
     * @brief Destructor
     *
     * Close the db connection
     */
    ~DbManager();
    QString tableNamesSettings = "tblGroup";

    typedef struct
    {
        int id;
        QString groupName;
        QString data;
        QString createdAt;
        QString updatedAt;
    } table_group;

    /**
     * @brief It is the place where struct structures created in a parent field are collected and called from the same place.
     */
    struct tablesAndColumns
    {

        table_group _tableGroup;
    };

    const tablesAndColumns *allTables;

    // Games struct
    struct Group
    {
        int id;
        QString groupName;
        QString data;
        QString createdAt;
        QString updatedAt;
    };


    QVector<Group> getGroup();
    QVector<Group> getGroupById(int gameId);

    bool insertGroup(const QString groupName, const QString &data);
    bool updateGroup(const QString groupName, const QString &data, const int &groupId);
    bool deleteGroup(int groupId);
    QVector<DbManager::Group> getGroups();
     QVector<DbManager::Group> getOneGroup(QString groupName);
};

#endif // DBMANAGER_H
