#include "dbmanager.h"
#include "qdatetime.h"
#include <QDebug>

DbManager::DbManager(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");

    m_db.setDatabaseName(path);

    allTables = new tablesAndColumns();

    if (!m_db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";

        QSqlQuery query;
        query.prepare("CREATE TABLE groups(id INTEGER PRIMARY KEY AUTOINCREMENT,groupName TEXT, data TEXT, createdAt TEXT, updatedAt TEXT);");

        if (!query.exec())
        {
            qDebug() << "Table group already created.";
        }
    }
}

DbManager::~DbManager()
{
    if (m_db.isOpen())
    {
        m_db.close();
    }
}

bool DbManager::insertGroup(const QString groupName, const QString &data)
{
    bool success = false;

    if (!groupName.isEmpty() && !data.isEmpty())
    {
        QSqlQuery queryAdd;
        queryAdd.prepare("INSERT INTO groups (groupName, data, createdAt, updatedAt) VALUES (:groupName, :data, :createdAt, :updatedAt)");
        queryAdd.bindValue(":groupName", groupName);
        queryAdd.bindValue(":data", data);

        QDate currentDate = QDate::currentDate();
        int year = currentDate.year();
        int month = currentDate.month();
        int day = currentDate.day();
        QString formattedDate = QString("%1-%2-%3").arg(year).arg(month, 2, 10, QChar('0')).arg(day, 2, 10, QChar('0'));

        queryAdd.bindValue(":createdAt", formattedDate);
        queryAdd.bindValue(":updatedAt", formattedDate);

        if (queryAdd.exec())
        {
            success = true;
        }
        else
        {
            qDebug() << "Could not insert to group: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "Data is required to add.";
    }

    return success;
}

bool DbManager::updateGroup(const QString groupName, const QString &data, const int &groupId)
{
    bool success = false;

    if (!groupName.isEmpty())
    {
        QSqlQuery queryAdd;
        queryAdd.prepare("UPDATE groups SET groupName = :groupName, data = :data, updatedAt = :updatedAt WHERE id = :groupId");
        queryAdd.bindValue(":groupName", groupName);
        queryAdd.bindValue(":data", data);

        QDate currentDate = QDate::currentDate();
        int year = currentDate.year();
        int month = currentDate.month();
        int day = currentDate.day();
        QString formattedDate = QString("%1-%2-%3").arg(year).arg(month, 2, 10, QChar('0')).arg(day, 2, 10, QChar('0'));

        queryAdd.bindValue(":updatedAt", formattedDate);
        queryAdd.bindValue(":groupId", groupId);

        if (queryAdd.exec())
        {
            success = true;
        }
        else
        {
            qDebug() << "Could update group: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "Data is required to update.";
    }

    return success;
}

// Games struct
struct Groups
{
    int id;
    QString groupName;
    QString data;
    QString createdAt;
    QString updatedAt;
};

QVector<DbManager::Group> DbManager::getGroups()
{
    QVector<Group> groups;

    QSqlQuery query("SELECT * FROM groups ORDER BY id DESC");
    int idIndex = query.record().indexOf("id");
    int groupNameIndex = query.record().indexOf("groupName");
    int dataIndex = query.record().indexOf("data");
    int createdAtIndex = query.record().indexOf("createdAt");
    int updatedAtIndex = query.record().indexOf("updatedAt");

    while (query.next())
    {
        int id = query.value(idIndex).toInt();
        QString groupName = query.value(groupNameIndex).toString();
        QString data = query.value(dataIndex).toString();

        QString createdAt = query.value(createdAtIndex).toString();
        QString updatedAt = query.value(updatedAtIndex).toString();

        groups.push_back(Group{id, groupName, data, createdAt, updatedAt});
    }

    return groups;
}

QVector<DbManager::Group>  DbManager::getOneGroup(QString groupName){
    QVector<Group> groups;

    QSqlQuery query;
    query.prepare("SELECT * FROM groups WHERE groupName = :name");
    query.bindValue(":name", groupName);
    qDebug() << groupName;


    if (query.exec())
    {
        int idIndex = query.record().indexOf("id");
        int groupNameIndex = query.record().indexOf("groupName");
        int dataIndex = query.record().indexOf("data");
        int createdAtIndex = query.record().indexOf("createdAt");
        int updatedAtIndex = query.record().indexOf("updatedAt");

        while (query.next())
        {
            int id = query.value(idIndex).toInt();
            QString groupName = query.value(groupNameIndex).toString();
            QString data = query.value(dataIndex).toString();

            QString createdAt = query.value(createdAtIndex).toString();
            QString updatedAt = query.value(updatedAtIndex).toString();

            groups.push_back(Group{id, groupName, data, createdAt, updatedAt});
        }
    }
    else
    {
        qDebug() << "Error executing query:" << query.lastError().text();
    }


    return groups;
}

bool DbManager::deleteGroup(int groupId)
{

    QSqlQuery query;
    query.prepare("DELETE FROM groups WHERE id = :groupId");
    query.bindValue(":groupId", groupId);

    if (query.exec())
    {
      return true;
    }else
    {
        qDebug() << "Error executing query int deleteGroup func:" << query.lastError().text();
        return false;
    }

    return false;
}




