#pragma once
#include <QObject>
#include <QString>
#include <QList>
#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDir>

// 用户数据结构
struct UserInfo {
    QString username;
    QString password;
    QString role; // "admin" 或 "user"
};

class UserManager : public QObject {
    Q_OBJECT

public:
    static UserManager& instance(); // 单例模式
    // 加载用户数据
    void loadUsers();
    // 核心功能：验证登录
    bool verifyUser(const QString& username, const QString& password, const QString& role);

private:
    UserManager();

    QList<UserInfo> m_users; // 内存中的用户列表
    QString m_jsonPath;      // JSON 文件路径
};
