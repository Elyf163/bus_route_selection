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

// 定义单一用户的数据结构
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
    // 输入：账号、密码、尝试登录的角色
    // 输出：是否验证通过
    bool verifyUser(const QString& username, const QString& password, const QString& role);

private:
    UserManager();

    QList<UserInfo> m_users; // 内存中的用户列表
    QString m_jsonPath;      // JSON 文件路径
};
